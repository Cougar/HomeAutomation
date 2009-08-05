//============================================================================
// Name        : Atom.cpp
// Author      : Mattias Runge
// Version     : 2.0
// Copyright   : GPL
//============================================================================

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <fstream>
#include <iterator>

#include <stdio.h>

#include "version.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
#include "broker/Broker.h"
#include "message/Header.h"
#include "message/Message.h"
#include "subscribers/monitor/Monitor.h"
#include "subscribers/cannet/CanNet.h"
#include "xml/Node.h"
#include "thread/Thread.h"
#include "net/UdpServer.h"
#include "xml/Node.h"
#include "protocol/Protocol.h"
#include "log/Logger.h"
#include "Exception.hpp"

namespace po = boost::program_options;

#define foreach         BOOST_FOREACH
#define reverse_foreach BOOST_REVERSE_FOREACH
using namespace std;


using namespace atom;

int main(int argc, char* argv[])
{
	try
	{
		log::Logger LOG;
		LOG.setName("Main");

		LOG.info("Atom (" + string(AutoVersion::FULLVERSION_STRING) + " " + string(AutoVersion::STATUS) + ")");
		LOG.info("Written by Mattias Runge 2009");

		Protocol::getInstance()->load("../../Configuration/protocol.xml", "../../Configuration/protocol-cache.xml");

		broker::Broker::pointer broker = broker::Broker::getInstance();

		subscribers::Monitor::pointer monitorSubscriber(new subscribers::Monitor(broker));
		//subscribers::CanNet::pointer canNetSubscriber(new subscribers::CanNet(broker, "192.168.1.250", 1100));

		LOG.info("Will now try to send a message...");

		message::Header header(0, "irTranceiver", "temperature");

		LOG.info("Header constructed...");

		message::Message::pointer message(new message::Message(header));

		LOG.info("Message constructed...");

		broker->put(message);

		sleep(10000000);
	}
	catch (Exception * e)
	{
		cout << "Exception: " << e->getDescription() << endl;
	}




	return 0;
	//udpSubscriber->write(utils::BitBuffer(startPacket));


	cout << "ABC123" << endl;

	return 0;
	/*sleep(1);

		messagebroker::Message message("");


		message.data = "test1";
		broker.put(message);

		message.setOrigin(monitorSubscriber);
		message.data = "test2";
		broker.put(message);
		sleep(1);
		delete monitorSubscriber;

		message.data = "test3";
		broker.put(message);
		sleep(1);
		delete udpSubscriber;


		try
		{
			// Declare a group of options that will be
			// allowed only on command line
			po::options_description generic("Generic options");
			generic.add_options()	("version,v", "print version string")
									("help", "produce help message");

			// Declare a group of options that will be
			// allowed both on command line and in
			// config file
			po::options_description config("Configuration");
			config.add_options()("optimization", po::value<int>()->default_value(10), "optimization level")
								("include-path,I", po::value<vector<string> >()->composing(), "include path");

			// Hidden options, will be allowed both on command line and
			// in config file, but will not be shown to the user.
			po::options_description hidden("Hidden options");
			hidden.add_options()("input-file", po::value<vector<string> >(), "input file");

			po::options_description cmdline_options;
			cmdline_options.add(generic).add(config).add(hidden);

			po::options_description config_file_options;
			config_file_options.add(config).add(hidden);

			po::options_description visible("Allowed options");
			visible.add(generic).add(config);

			po::positional_options_description p;
			p.add("input-file", -1);

			po::variables_map vm;
			store(po::command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm);

			ifstream ifs("multiple_sources.cfg");
			store(parse_config_file(ifs, config_file_options), vm);
			notify(vm);

			if (vm.count("help"))
			{
				cout << visible << "\n";
				return 0;
			}

			if (vm.count("version"))
			{
				cout << "Multiple sources example, version 1.0\n";
				return 0;
			}

			if (vm.count("include-path"))
			{
				//cout << "Include paths are: " << vm["include-path"].as<vector<string>>() << "\n";
			}

			if (vm.count("input-file"))
			{
				//cout << "Input files are: " << vm["input-file"].as<vector<string>>() << "\n";
			}

			//cout << "Optimization level is " << opt << "\n";

	*/
	/*if (argc != 3)
	{
		std::cout << "Usage: async_client <server> <path>\n";
		std::cout << "Example:\n";
		std::cout << "  async_client www.boost.org /LICENSE_1_0.txt\n";
		return 1;
	}

	boost::asio::io_service io_service;
	client c(io_service, argv[1], argv[2]);
	io_service.run();*/
	/*
			 std::string hello( "Hello, world!" );

			foreach( char ch, hello )
			{
				std::cout << ch;
			}

		}
		catch (std::exception& e)
		{
			std::cout << "Exception: " << e.what() << "\n";
		}
	*/
	return 0;
}
