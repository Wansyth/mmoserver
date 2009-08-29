/*
---------------------------------------------------------------------------------------
This source file is part of swgANH (Star Wars Galaxies - A New Hope - Server Emulator)
For more information, see http://www.swganh.org


Copyright (c) 2006 - 2008 The swgANH Team

---------------------------------------------------------------------------------------
*/

#ifndef ANH_ZONESERVER_TRADEMANAGER_H
#define ANH_ZONESERVER_TRADEMANAGER_H

#include "Common/MessageDispatchCallback.h"
#include "DatabaseManager/DatabaseCallback.h"
#include "ObjectFactory.h"
#include "ZoneOpcodes.h"
#include "CharacterBuilderTerminal.h"
#include "TreasuryManager.h"

#include <vector>

#define 	gTradeManager	TradeManager::getSingletonPtr()

//======================================================================================================================

class Message;
class Database;
class MessageDispatch;


enum TRMQueryType
{
	TRMQuery_NULL						=	0,
	TRMQuery_LoadBazaar					=	1,
	TRMQuery_AuctionQuery				=	2,
	TRMQuery_CreateAuction				=	3,
	TRMQuery_GetDetails					=	4,
	TRMQuery_CancelAuction				=	5,
	TRMQuery_RetrieveAuction			=	6,
	TRMQuery_DeleteAuction				=	7,
	TRMQuery_BidAuction					= 8,
	TRMQuery_ACKRetrieval				= 9,
	TRMQuery_MoneyTransaction			= 10,
	TRMQuery_BazaarMoneyTransaction		= 11,
	TRMQuery_CreateAuctionTransaction	= 12,
	TRMQuery_ItemTableFrogQuery			= 13,
	TRMQuery_CheckListingsBazaar		= 14,
};
enum TRMVendorType
{
	TRMVendor_bazaar	=	1,
	TRMVendor_player    =   2,
	
};

enum TRMAuctionType: uint32
{
	TRMVendor_Auction   	=	0,
	TRMVendor_Instant       =   1,
	TRMVendor_Cancelled		=   2,
	TRMVendor_Offer 		=   3,
	TRMVendor_Sold  		=   4,
	TRMVendor_NotSold 		=   5,
	
};



enum TRMAuctionWindowType
{ 
	TRMVendor_AllAuctions	=	2,
	TRMVendor_MySales       =   3,
	TRMVendor_MyBids		=   4,
	TRMVendor_AvailableItems=   5,
	TRMVendor_Offers		=   6,
	TRMVendor_ForSale		=   7,
	TRMVendor_Stockroom		=   8,

};

enum TRMPermissionType
{
	TRMOwner        	=	0,
	TRMNotOwner         =   1,
	TRMBazaar           =   2,
	TRMBazaarQuery		=	3,

};

enum TRMRegionType
{
	TRMGalaxy        	=	0,
	TRMPlanet	        =   1,
	TRMRegion           =   2,
	TRMVendor			=	3,

};

//======================================================================================================================

class TradeManagerAsyncContainer
{
public:

	TradeManagerAsyncContainer(TRMQueryType qt,DispatchClient* client){ mQueryType = qt; mClient = client; }
	~TradeManagerAsyncContainer(){}

	TRMQueryType		mQueryType;
	DispatchClient*		mClient;	

	uint64				AuctionID;
	uint32				BazaarWindow;
	uint32				BazaarPage;
	uint64				BazaarID;
	uint32				MyBid;
	uint32				MyProxy;
	TangibleObject*		tangible;
	uint32				crc;
	uint32				time;
	string				name;
	string				mPlanet;
	string				mRegion;
	string				mOwner;

	//trading
	int32				amount1,amount2;//inv credits
	int32				amountbank,amountcash;
	int32				bank1;//bankcredits
	int32				bank2;
	PlayerObject*		player1;
	PlayerObject*		player2;
	uint64				sellerID;
	int32				mX;
	int32				mY;
	
	//TRMVendorType		mVendorType;
	TRMAuctionType		auctionType;

	uint8				premium;
	uint32				itemType;
	uint32				price;
	string				description;
	string				tang;
};

//======================================================================================================================

struct DescriptionItem
{
		uint64			ItemID;
		int8			Description[1024];
		int8			tang[128];
		int8			details[1024];
};

//thats were the raw Auction db data goes
struct AuctionItem
{
		uint64			ItemID;
		uint64			OwnerID;
		uint64			BazaarID;
		uint32          AuctionTyp;
		uint64			EndTime;
		uint32          Premium;
		uint32			Category;
		uint32			ItemTyp;
		uint32			Price;
		int8			Name[128];
		int8			Description[1024];
		uint16			RegionID;
		uint64			BidderID;
		uint16			PlanetID;
		int8			SellerName[32];
		int8			BazaarName[128];
		uint32			MyBid;
		uint32			MyProxy;
		
};


class Vendor
{
		long			ownerid;
		long			id;
		uint32			regionid;
		uint32			planetid;
		int8			string[128];

};

 
//======================================================================================================================

class TradeManager : public MessageDispatchCallback, public DatabaseCallback
{
	friend class ObjectFactory;
	public:

		static TradeManager*	getSingletonPtr() { return mSingleton; }
		static TradeManager*	Init(Database* database,MessageDispatch* dispatch);

		~TradeManager();

		void				Shutdown();

		virtual void		handleDispatchMessage(uint32 opcode,Message* message,DispatchClient* client);
		virtual void		handleDatabaseJobComplete(void* ref,DatabaseResult* result);

		void				addTradeInvitation(PlayerObject* invitedPlayer, PlayerObject* invitingPlayer);

		ItemFrogClass		mItemFrogClass;

	private:

		TradeManager(Database* database,MessageDispatch* dispatch);

		void 				_processBankTipDeduct(Message* message,DispatchClient* client);
		void 				_processFindFriendCreateWaypointMessage(Message* message,DispatchClient* client);
		void 				_processFindFriendRequestPositionMessage(Message* message,DispatchClient* client);

		void				_processBanktipUpdate(Message* message,DispatchClient* client);
		
		void				_processHandleAuctionCreateMessage(Message* message,DispatchClient* client,TRMAuctionType auction);
		void 				_processDeductMoneyMessage(Message* message,DispatchClient* client);
		

		void				_processCancelLiveAuctionMessage(Message* message,DispatchClient* client);
		void				_processCreateItemMessage(Message* message,DispatchClient* client);
		void				checkPlacedInstrument(Item* addedItem, DispatchClient* client);
		
		//Trade
		void				_processAbortTradeMessage(Message* message,DispatchClient* client);
		void				_processTradeCompleteMessage(Message* message,DispatchClient* client);
		void				_processAddItemMessage(Message* message,DispatchClient* client);
		void				informTradePartner(TangibleObject* Item,PlayerObject* TradePartner);
		void				_processRemoveItemMessage(Message* message,DispatchClient* client);
		void				_processAcceptTransactionMessage(Message* message,DispatchClient* client);
		void				_processUnacceptTransactionMessage(Message* message,DispatchClient* client);
		void				_processBeginVerificationMessage(Message* message,DispatchClient* client);
		void				_processVerificationMessage(Message* message,DispatchClient* client);
		void				_processGiveMoneyMessage(Message* message,DispatchClient* client);
		void				TradeTransaction(DispatchClient* client,PlayerObject* player1,PlayerObject* player2);


		static TradeManager*		mSingleton;
		static bool					mInsFlag;

		Database*					mDatabase;
		MessageDispatch*			mMessageDispatch;
		TRMPermissionType			mPermissionTyp;
		
		uint32						mErrorCount;
		
		uint32						mZoneId;

			
};



#endif 
