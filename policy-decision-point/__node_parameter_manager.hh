#pragma once

#include<map>
#include<string>
#include<boost/thread.hpp>

#include "node_parameter.hh"

struct NodeParameterManagerNode{
	NodeParameters * _nodeParameters;
	boost::shared_mutex _mutex;

	NodeParameterManagerNode(NodeParameters * p = NULL):_nodeParameters(p){}
	~NodeParameterManagerNode(){if (_nodeParameters) delete _nodeParameters;}
};

template<typename Synchronizer>
class __NodeParameterManager: public Synchronizer{
	public:

		static __NodeParameterManager<Synchronizer> * getInstance(){
			static Guard g;

			if (instance == NULL)
				instance = new __NodeParameterManager<Synchronizer>();

			return instance;
		}

		//policy def version number may be added later
		NodeParameterManagerNode * getNodeParameters(const std::string & server, pol_def_version_t version);

		//inline std::map<std::string, NodeParameterManagerNode *>::iterator getNodeParameterListBegin(pol_def_version_t version){return node_param_list.begin();}
		//inline std::map<std::string, NodeParameterManagerNode *>::iterator getNodeParameterListEnd(pol_def_version_t version){return node_param_list.end();}

	private:
		class Guard{
			public:
				~Guard(){
					if (instance != NULL)
						delete instance;
				}
		};


		__NodeParameterManager<Synchronizer>(){}
		__NodeParameterManager<Synchronizer>(__NodeParameterManager<Synchronizer> &){}

		~__NodeParameterManager(){
			for (auto it = node_param_list.begin(); it != node_param_list.end(); ++it){
				boost::upgrade_lock<boost::shared_mutex> lock(it->second->_mutex);
				boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
				delete it->second->_nodeParameters;
			}

			node_param_list.clear();
		}

		NodeParameterManagerNode * load(const std::string & server, pol_def_version_t version);
		std::map<std::string, NodeParameterManagerNode *> node_param_list;

		static __NodeParameterManager<Synchronizer> * instance;

};

template<typename Synchronizer>
__NodeParameterManager<Synchronizer> * __NodeParameterManager<Synchronizer>::instance = NULL;

template<typename Synchronizer>
NodeParameterManagerNode * __NodeParameterManager<Synchronizer>::getNodeParameters(const std::string & server, pol_def_version_t version){

	try{
		return node_param_list.at(server);
	}
	catch (std::out_of_range e){
		return load(server, version);
	}
}

template<typename Synchronizer>
NodeParameterManagerNode * __NodeParameterManager<Synchronizer>::load(const std::string & server, pol_def_version_t version){
	NodeParameterManagerNode * node = new NodeParameterManagerNode(new NodeParameters());
	std::string params_str = Synchronizer::synchronize(server, version);
	if (params_str == "")
		return NULL;

	node->_nodeParameters->load(params_str, version);
	node_param_list.insert(std::pair<std::string, NodeParameterManagerNode *>(server, node));
	return node;
}
