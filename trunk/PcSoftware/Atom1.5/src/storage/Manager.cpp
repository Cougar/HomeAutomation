/*
 * 
 *  Copyright (C) 2010  Mattias Runge
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * 
 */

#include "Manager.h"

namespace atom {
namespace storage {

Manager::Pointer Manager::instance_;

Manager::Manager() : LOG("storage::Manager")
{
}

Manager::~Manager()
{
    for (StoreList::iterator it = this->stores_.begin(); it != this->stores_.end(); it++)
    {
        try
        {
            it->second->Flush();
        }
        catch (std::runtime_error& e)
        {
            LOG.Error("Error while flusing store, " + std::string(e.what()));
        }
    }
}

Manager::Pointer Manager::Instance()
{
    return Manager::instance_;
}

void Manager::Create()
{
    Manager::instance_ = Manager::Pointer(new Manager());
}

void Manager::Delete()
{
    Manager::instance_.reset();
}

void Manager::SetRootPath(std::string root_path)
{
    this->root_path_ = root_path;
}

void Manager::SetStoreFlushPolicy(std::string store_name, Store::FlushPolicy flush_policy)
{
    StoreList::iterator it = this->stores_.find(store_name);
    
    if (it == this->stores_.end())
    {
        LOG.Info("Loading storage " + store_name + "...");
        this->stores_[store_name] = Store::Pointer(new Store(this->root_path_ + store_name));
    }
    
    this->stores_[store_name]->SetFlushPolicy(flush_policy);
}

void Manager::FlushStore(std::string store_name)
{
    StoreList::iterator it = this->stores_.find(store_name);
    
    if (it == this->stores_.end())
    {
        LOG.Info("Loading storage " + store_name + "...");
        this->stores_[store_name] = Store::Pointer(new Store(this->root_path_ + store_name));
    }
    
    this->stores_[store_name]->Flush();
}

Store::ParameterList& Manager::GetParameters(std::string store_name)
{
    StoreList::iterator it = this->stores_.find(store_name);
    
    if (it == this->stores_.end())
    {
        LOG.Info("Loading storage " + store_name + "...");
        this->stores_[store_name] = Store::Pointer(new Store(this->root_path_ + store_name));
    }
    
    return this->stores_[store_name]->GetParameters();
}

std::string Manager::GetParameter(std::string store_name, std::string parameter_name)
{
    StoreList::iterator it = this->stores_.find(store_name);
    
    if (it == this->stores_.end())
    {
        LOG.Info("Loading storage " + store_name + "...");
        this->stores_[store_name] = Store::Pointer(new Store(this->root_path_ + store_name));
    }
    
    return this->stores_[store_name]->GetParameter(parameter_name);
}

void Manager::SetParameter(std::string store_name, std::string parameter_name, std::string parameter_value)
{
    StoreList::iterator it = this->stores_.find(store_name);
    
    if (it == this->stores_.end())
    {
        LOG.Info("Loading storage " + store_name + "...");
        this->stores_[store_name] = Store::Pointer(new Store(this->root_path_ + store_name));
    }
    
    return this->stores_[store_name]->SetParameter(parameter_name, parameter_value);
}

}; // namespace timer
}; // namespace atom
