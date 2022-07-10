/*
 * OrderHouse.h
 *
 *  Created on: 2022Äê7ÔÂ9ÈÕ
 *      Author: yufei
 */

#ifndef ORDERHOUSE_H_
#define ORDERHOUSE_H_

#include <string>
#include <unordered_map>
#include <mutex>
#include <condition_variable>

#include "Order.h"
#include "OrderEvent.h"

namespace ENGINE {

//a simple lock to protect order house
class OrderHouseLock {
public:
	OrderHouseLock();
	OrderHouseLock(const OrderHouseLock&);
	void readWait();
	void readDone();
	void writeWait();
	void writeDone();
private:
	std::mutex myMutex;
	std::condition_variable myReaderSignal;
	std::condition_variable myWriterSignal;
	unsigned int myReader;
	bool myWriter;
};

class OrderHouse {
public:
	OrderHouse() = default;
	virtual ~OrderHouse() = default;

	/**
	 * Add a new order into order house
	 * return a order's exchange reference
	 */
	std::string addOrder(const Order&);
	void executeOrder(const std::string& exchangeId, int execQuantity);
	void cancelOrder (const std::string& exchangeId);
	OrderEvent queryOrder (const std::string& exchangeId) ;
private:
	/**
	 * generate next exchangeId
	 */
	std::string nextExchangeId();
private:
	//exchangeId, Order
	//contains all order's latest information
	std::unordered_map<std::string, Order> myOrderBook;

	OrderHouseLock myLocker;

};

} /* namespace ENGINE */

#endif /* ORDERHOUSE_H_ */
