#pragma once

#include "options.hh"
#include<map>
#include<string>
#include<vector>
#include<boost/thread.hpp>

class SimpleSynchronizer{
	public:
		void addParamsString(const std::string & server, const std::string & params_str){
			boost::upgrade_lock<boost::shared_mutex> lock(_mutex);
			boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
			params_list.insert(std::pair<std::string, std::string>(server, params_str));
		}

		std::string synchronize(const std::string & server, pol_def_version_t version){
			try{
				boost::shared_lock<boost::shared_mutex> lock(_mutex);
				return params_list.at(server);
			}
			catch(std::out_of_range e){
				return "";
			}
		}

		std::vector<std::string> getServerList(){
			std::vector<std::string> v;
			boost::shared_lock<boost::shared_mutex> lock(_mutex);
			for (auto it = params_list.begin();it != params_list.end();++it)
				v.push_back(it->first);

			return v;
		}
	private:
		std::map<std::string, std::string> params_list;
		boost::shared_mutex _mutex;
};
