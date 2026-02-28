#include "CheckMailAction.h"

#include <sstream>

#include "DatabaseEnv.h"
#include "Mail.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"

bool CheckMailAction::Execute(Event /*event*/)
{
    Player* bot = botAI->GetBot();
    if (!bot)
        return false;

    if (!bot->m_mailsLoaded)
        return false;

    // Copy mails first; ProcessMail may mutate mail state.
    std::vector<Mail*> mails;
    for (auto it = bot->GetMailBegin(); it != bot->GetMailEnd(); ++it)
        if (Mail* m = *it)
            mails.push_back(m);

    std::vector<uint32> toDelete;
    for (Mail* mail : mails)
    {
        if (!mail || mail->state == MAIL_STATE_DELETED)
            continue;

        // Only process delivered mail.
        if (mail->deliver_time > time(nullptr))
            continue;

        uint32 senderLow = mail->sender;
        if (!senderLow)
            continue;

        ProcessMail(mail, senderLow);

        // CMaNGOS behavior: delete processed mail after handling.
        toDelete.push_back(mail->messageID);
        mail->state = MAIL_STATE_DELETED;
    }

    for (uint32 id : toDelete)
    {
        bot->SendMailResult(id, MAIL_DELETED, MAIL_OK);
        CharacterDatabase.PExecute("DELETE FROM mail WHERE id = %u", id);
        CharacterDatabase.PExecute("DELETE FROM mail_items WHERE mail_id = %u", id);
        bot->RemoveMail(id);
    }

    return true;
}

bool CheckMailAction::isUseful()
{
    Player* bot = botAI->GetBot();
    if (!bot)
        return false;

    // Only when not actively controlled by a real player.
    if (botAI->HasActivePlayerMaster())
        return false;

    if (bot->InBattleground())
        return false;

    if (!bot->m_mailsLoaded || bot->GetMailSize() == 0)
        return false;

    return true;
}

void CheckMailAction::ProcessMail(Mail* mail, uint32 senderLow)
{
    Player* bot = botAI->GetBot();
    if (!bot || !mail)
        return;

    if (mail->items.empty())
        return;

    // Don't bounce back "requested" items (CMaNGOS convention).
    if (mail->subject.find("Item(s) you asked for") != std::string::npos)
        return;

    // Don't touch auction mail.
    if (mail->messageType != MAIL_NORMAL || mail->stationery == MAIL_STATIONERY_AUCTION)
        return;

    ObjectGuid senderGuid(HighGuid::Player, senderLow);

    // Return the first attached item (CMaNGOS behavior).
    for (MailItemInfo const& mi : mail->items)
    {
        Item* item = bot->GetMItem(mi.item_guid);
        if (!item)
            continue;

        std::ostringstream body;
        body << "Hello,\n\nHere are the item(s) you've sent me by mistake.\n\nThanks,\n" << bot->GetName() << "\n";

        CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_MAIL_BY_ID);
        stmt->setUInt32(0, mail->messageID);
        trans->Append(stmt);

        stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_MAIL_ITEM_BY_ID);
        stmt->setUInt32(0, mail->messageID);
        trans->Append(stmt);

        MailDraft draft("Item(s) you've sent me", body.str());
        bot->RemoveMItem(mi.item_guid);
        item->SetOwnerGUID(senderGuid);
        item->SaveToDB(trans);
        draft.AddItem(item);

        draft.SendMailTo(trans,
            MailReceiver(ObjectAccessor::FindConnectedPlayer(senderGuid), senderLow),
            MailSender(bot));

        CharacterDatabase.CommitTransaction(trans);
        return;
    }
}
