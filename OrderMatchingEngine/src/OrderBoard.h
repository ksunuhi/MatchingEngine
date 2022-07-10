/*
 * OrderBoard.h
 *
 *  Created on: 2022Äê7ÔÂ9ÈÕ
 *      Author: yufei
 */

#ifndef ORDERBOARD_H_
#define ORDERBOARD_H_

#include <string>
#include <map>
#include <unordered_map>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

#include "Order.h"
#include "OrderEvent.h"
#include "OrderHouse.h"

/**
 * How to match orders on the execution book?
 * construct a buy order list and a sell order list, both are sorted by sortKey explained later.
 * then when new order comes, goes to the opposite side list, execute from head of list
 *
 * For buy orders, since higher price means better price, use remaining price (max price - original price)
 * + time as sorting key
 * Buy order list (9 byte price + 9 bytes timestamp)
 * 000000000 0903  (buy @market price came at 09:03)
 * 999999900 0901  (buy @100 came at 09:01)
 * 999999900 0902  (buy @100 came at 09:02)
 * 999999901 0901  (buy @99 came at 09:02)
 *
 * For sell orders, lower price means better price, use its orignal price + time as sorting key
 * Sell order list
 * 000000000 0903  (sell @market price came at 09:03)
 * 000000100 0901  (sell @100 came at 09:01)
 * 000000100 0902  (sell @100 came at 09:02)
 * 000000101 0901  (sell @101 came at 09:01)
 *
 * If come in buy order with price 100, then target sell order matching string is
 * 000000100 9999
 * any records lower than this matching string are matching candidates
 *
 * If come in sell order with price 101, then target buy order matching string is
 * 999999899 9999
 * any records lower than this matching string are matching candidate
 */
namespace ENGINE {

enum class RequestType_t {
	new_e,
	cancel_e
};

struct OrderRequest {
	OrderRequest(RequestType_t requestType, const Order& order);
	RequestType_t myRequestType;
	Order myOrder;
};

class OrderBoard {
public:
	OrderBoard();
	OrderBoard(const OrderBoard& ) = delete;
	virtual ~OrderBoard();

	void submitNew(const Order&);
	void submitCancel(const Order&);
	OrderEvent getResultsOfLastOperation(const Order&);

private:
	/**
	 *Create a string as sort key for sorting order on the execution book
	 *format:
	 * 9 digit (price) + 9 digit (sequence)
	 * price (9 digit) -
	 *       buy order:
	 *           marketOrder - price (000000000)
	 *           limit order - 99999999-price	 *
	 *       sell order
	 *           marketOrder - price (000000000)
	 *           limit order - price
	 *
	 */
	std::string createOrderSortKey(const Order&);

	/**
	 *Create a string as matching key to look matching orders
	 *format:
	 * 9 digit (price) + 9 digit (sequence)
	 * price (9 digit) -
	 *       buy order:
	 *           marketOrder - 99999999
	 *           limit order - price
	 *       sell order
	 *           marketOrder - 99999999
	 *           limit order - 99999999-price
	 * sequence (9 digit) - always set to 99999999
	 */
	std::string createOrderMachingKey(const Order&) const;

	/**
	 * Match the order with existing orders on the board, and
	 * place remaining quantity on the board.
	 * It may change order status to
	 * newAck_e
	 * newReject_e (market order can not get executed)
	 * execution_e
	 */
	void newOrder(const Order&);
	void cancelOrder(const std::string& clientId);
	void processMsg();

private:

	//a sorted order list to be executed
	//sortKey, {exchangeId, living quantity}
	typedef std::map<std::string, std::pair<std::string,int>> ExecutionBook;

	//execution book is organized by instrument, each instrument
	//has a buy order list and a sell order list
	std::unordered_map<int, std::pair<ExecutionBook,ExecutionBook>> myExecutionBooks;

	//contains all order's latest information
	OrderHouse myOrderHouse;

	//sequence to represent order arrival time priority
	unsigned int myOrderSequence;

	//a map of client id and exchangeId
	//since internally, it use exchangeId to manage orders, this class
	//keeps the boundary of clientId and exchangeId
	std::unordered_map<std::string, std::string> myClientIdMap;

	//a map of client id and order sort key on the execution board
	//use to remove order from the exeucton book when order is cancelled
	std::unordered_map<std::string, std::string> myClientIdSortKeyMap;

	//build a queue between front and back, then
	std::queue<OrderRequest> myRequestQueue;
	std::thread  myQueueReader;
	std::condition_variable myQueueSignal;
	std::mutex myQueueMutex;
	bool myStopFlag;
};

} /* namespace ENGINE */

#endif /* ORDERBOARD_H_ */
