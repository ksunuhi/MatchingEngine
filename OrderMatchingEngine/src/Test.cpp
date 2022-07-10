//============================================================================
// Name        : OrderMatchingEngine.cpp
// Author      : Me
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <vector>
#include <cassert>
#include <thread>
#include <chrono>

#include "MatchingEngine.h"

using namespace std;
using namespace ENGINE;

/**
 * a helper function
 */
void check(const OrderEvent& event, OrderStatus_t status, int quantityDone) {
	assert(event.getStatus() == status);
	assert(event.getQuantityDone() == quantityDone);
	cout << "check "<< event.getClientId() << " ok "<<endl;
}

/** case of 1 limit sell order to match 3 limit buy orders***
 * step1
 * send 3 buy orders to engine
 *    order1 buy 1000@101
 *    order2 buy 2000@102
 *    order3 buy 3000@103
 * send 1 sell order to engine
 *    order4 sell 5600@101
 * then it expectes
 *    order1 - parcial fill, remaing 400
 *    order2 - full fill
 *    order3 - full fill
 *    order4 - full fill
 * step2
 *    cancel these 4 orders, expects
 *    ordre1-cancel ack
 *    others- cancel nack (because fully filled)
 */
void testcase1() {
	MatchingEngine& engine = MatchingEngine::getInstance();
	int instrument = 6758; //sony

	//set 3 buy ordes
	Order order1("order1", instrument,OrderSide_t::buy_e,101,1000);
	Order order2("order2", instrument,OrderSide_t::buy_e,102,2000);
	Order order3("order3", instrument,OrderSide_t::buy_e,103,3000);

	//1 sell to fill
	Order order4("order4", instrument,OrderSide_t::sell_e,101,5600);

	engine.submitNew(order1);
	engine.submitNew(order2);
	engine.submitNew(order3);
	engine.submitNew(order4);

	//wait a bit, then check result
	std::this_thread::sleep_for (std::chrono::seconds(5));

	//check executions
	check(engine.getResultsOfLastOperation(order1),OrderStatus_t::execution_e,600);
	check(engine.getResultsOfLastOperation(order2),OrderStatus_t::execution_e,2000);
	check(engine.getResultsOfLastOperation(order3),OrderStatus_t::execution_e,3000);
	check(engine.getResultsOfLastOperation(order4),OrderStatus_t::execution_e,5600);

	//cancel all of them
	engine.submitCancel(order1);
	engine.submitCancel(order2);
	engine.submitCancel(order3);
	engine.submitCancel(order4);
	std::this_thread::sleep_for (std::chrono::seconds(5));

	//check cancel result
	check(engine.getResultsOfLastOperation(order1),OrderStatus_t::cancelAck_e,600);
	check(engine.getResultsOfLastOperation(order2),OrderStatus_t::cancelReject_e,2000);
	check(engine.getResultsOfLastOperation(order3),OrderStatus_t::cancelReject_e,3000);
	check(engine.getResultsOfLastOperation(order4),OrderStatus_t::cancelReject_e,5600);
}

/**
 * step1
 * send 3 buy orders to engine
 *    order1 sell 1000@101
 *    order2 sell 2000@102
 *    order3 sell 3000@103
 * send 1 sell order to engine
 *    order4 buy 1000@100
 *    order5 buy 3500@102
 * then it expectes
 *    order1 - full fill
 *    order2 - full fill
 *    order3 - no fill
 *    order4 - no fill
 *    order5 - partical fill,500 remaining on the board
 * step2
 *    order6 - buy market price, 10000 to fill order3
 *    expectes
 *      order3 - full fill
 *      order4 - no fill
 *      order5 - partical fill,500 remaining on the board (limit order)
 *      order6 - partial fill, but not remain on board (market order)
 *
 */
void testcase2() {
	MatchingEngine& engine = MatchingEngine::getInstance();
	int instrument = 6759;

	//set 3 buy ordes
	Order order1("order1", instrument,OrderSide_t::sell_e,101,1000);
	Order order2("order2", instrument,OrderSide_t::sell_e,102,2000);
	Order order3("order3", instrument,OrderSide_t::sell_e,103,3000);

	//1 sell to fill
	Order order4("order4", instrument,OrderSide_t::buy_e,100,1000);
	Order order5("order5", instrument,OrderSide_t::buy_e,102,3500);

	engine.submitNew(order1);
	engine.submitNew(order2);
	engine.submitNew(order3);
	engine.submitNew(order4);
	engine.submitNew(order5);

	//wait a bit, then check result
	std::this_thread::sleep_for (std::chrono::seconds(5));

	//check executions
	check(engine.getResultsOfLastOperation(order1),OrderStatus_t::execution_e,1000);
	check(engine.getResultsOfLastOperation(order2),OrderStatus_t::execution_e,2000);
	check(engine.getResultsOfLastOperation(order3),OrderStatus_t::newAck_e,0);
	check(engine.getResultsOfLastOperation(order4),OrderStatus_t::newAck_e,0);
	check(engine.getResultsOfLastOperation(order5),OrderStatus_t::execution_e,3000);

	//send a big market order to fill up all
	Order order6("order6", instrument,OrderSide_t::buy_e,0,10000);
	engine.submitNew(order6);
	std::this_thread::sleep_for (std::chrono::seconds(5));

	check(engine.getResultsOfLastOperation(order3),OrderStatus_t::execution_e,3000);

	engine.submitCancel(order4);
	engine.submitCancel(order5);
	engine.submitCancel(order6);
}

/** performance test***
 * step1
 * send 10000 buy orders to engine
 * send 10000 buy orders to engine
 */
void sendLargeOder() {
	MatchingEngine& engine = MatchingEngine::getInstance();
	int orderNum = 10000;
	int instrument = 6760;
	int base_price= 100;
	int base_quantity= 900;

	for(int i=0; i <orderNum; i++) {
		Order order("order"+to_string(i), instrument,OrderSide_t::buy_e,base_price+(i%100),base_quantity+(i%10));
		engine.submitNew(order);
	}

	for(int i=0; i <orderNum; i++) {
		Order order("order"+to_string(i), instrument,OrderSide_t::sell_e,base_price+(i%100),base_quantity+(i%10));
		engine.submitNew(order);
	}
}


void testcase3() {
	thread t1(sendLargeOder);
	thread t2(sendLargeOder);
	thread t3(sendLargeOder);
	t1.join();
	t2.join();
	t3.join();
	std::this_thread::sleep_for (std::chrono::seconds(10));
}

int main() {
	cout << "run test case1" << endl;
	testcase1();

	cout << "run test case2" << endl;
	testcase2();

	cout << "run test case3" << endl;
	testcase3();

	return 0;
}
