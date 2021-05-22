#include "LinkStateTable.h"

bool LinkStateTable::update(Message message)
{
    bool is_changed = false;
    bool is_find = false;
    if (message.cost == MAX_COST)
    {
        if (is_in_table(message.source_ip_addr, message.dest_ip_addr))
        {
            erase(message.source_ip_addr, message.dest_ip_addr);
            return true;
        }
        return false;
    }
    for (auto it = link_state_table_.begin(); it != link_state_table_.end(); it++)
    {
        if (it->empty())
        {
            link_state_table_.erase(it);
            continue;
        }
        if ((*it)[0].source_ip_addr == message.source_ip_addr)
        {
            for (auto j = it->begin(); j != it->end(); j++)
            {
                if (j->dest_ip_addr == message.dest_ip_addr)
                {
                    is_find = true;
                    if (j->cost != message.cost)
                    {
                        j->cost = message.cost;
                        is_changed = true;
                        break;
                    }
                }
            }
        }
        else if ((*it)[0].source_ip_addr == message.dest_ip_addr)
        {
            for (auto j = it->begin(); j != it->end(); j++)
            {
                if (j->dest_ip_addr == message.source_ip_addr)
                {
                    is_find = true;
                    if (j->cost != message.cost)
                    {
                        j->cost = message.cost;
                        is_changed = true;
                        break;
                    }
                }
            }
        }
    }
    if (!is_find)
    {
        is_changed = true;
        push(message.source_ip_addr, message.dest_ip_addr, message.cost);
    }
    if (is_changed)
    {
        print();
    }
    return is_changed;
}

void LinkStateTable::push(long source_ip_addr, long dest_ip_addr, int cost)
{
    bool is_find_source = false;
    bool is_find_dest = false;
    for (auto it = link_state_table_.begin(); it != link_state_table_.end(); it++)
    {
        if (it->empty())
        {
            link_state_table_.erase(it);
            continue;
        }
        if ((*it)[0].source_ip_addr == source_ip_addr)
        {
            is_find_source = true;
            it->push_back(LinkState(source_ip_addr, dest_ip_addr, cost));
        }
        if ((*it)[0].source_ip_addr == dest_ip_addr)
        {
            is_find_dest = true;
            it->push_back(LinkState(dest_ip_addr, source_ip_addr, cost));
        }
    }
    if (!is_find_source)
    {
        vector<LinkState> source_table;
        source_table.push_back(LinkState(source_ip_addr, dest_ip_addr, cost));
        link_state_table_.push_back(source_table);
    }
    if (!is_find_dest)
    {
        vector<LinkState> dest_table;
        dest_table.push_back(LinkState(dest_ip_addr, source_ip_addr, cost));
        link_state_table_.push_back(dest_table);
    }
}

void LinkStateTable::erase(long source_ip_addr, long dest_ip_addr)
{
    for (auto it = link_state_table_.begin(); it != link_state_table_.end(); it++)
    {
        if (it->empty())
        {
            link_state_table_.erase(it);
            continue;
        }
        if ((*it)[0].source_ip_addr == source_ip_addr)
        {
            for (auto j = it->begin(); j != it->end(); j++)
            {
                if (j->dest_ip_addr == dest_ip_addr)
                {
                    it->erase(j);
                    break;
                }
            }
        }
        else if ((*it)[0].source_ip_addr == dest_ip_addr)
        {
            for (auto j = it->begin(); j != it->end(); j++)
            {
                if (j->dest_ip_addr == source_ip_addr)
                {
                    it->erase(j);
                    break;
                }
            }
        }
    }
}

bool LinkStateTable::is_in_table(long source_ip_addr, long dest_ip_addr)
{
    for (auto it = link_state_table_.begin(); it != link_state_table_.end(); it++)
    {
        if (it->empty())
        {
            link_state_table_.erase(it);
            continue;
        }
        if ((*it)[0].source_ip_addr == source_ip_addr)
        {
            for (auto j = it->begin(); j != it->end(); j++)
            {
                if (j->dest_ip_addr == dest_ip_addr)
                {
                    return true;
                }
            }
        }
        else if ((*it)[0].source_ip_addr == dest_ip_addr)
        {
            for (auto j = it->begin(); j != it->end(); j++)
            {
                if (j->dest_ip_addr == source_ip_addr)
                {
                    return true;
                }
            }
        }
    }
    return false;
}
vector<LinkStateTable::LinkState> &LinkStateTable::operator[](int index)
{
    if (index >= 0 && index < link_state_table_.size())
        return link_state_table_[index];
    cerr << "Link State Table : out of range" << endl;
    return *(vector<LinkState> *)NULL;
}
void LinkStateTable::print()
{
    cout << "Link State Table:" << endl;
    cout << std::left << setw(LEN3) << "Source Ip Addr"
         << std::left << setw(LEN3) << "Dest Ip Addr"
         << std::left << setw(LEN3) << "Cost" << endl;
    for (int i = 0; i < 3 * LEN3; i++)
    {
        cout << "-";
    }
    cout << endl;
    for (auto it = link_state_table_.begin(); it != link_state_table_.end(); it++)
    {
        if (it->empty())
        {
            link_state_table_.erase(it);
            continue;
        }
        for (auto j = it->begin(); j != it->end(); j++)
        {
            cout << std::left << setw(LEN3) << inet_ntoa(*((in_addr *)&j->source_ip_addr))
                 << std::left << setw(LEN3) << inet_ntoa(*((in_addr *)&j->dest_ip_addr))
                 << std::left << setw(LEN3) << j->cost << endl;
        }
    }
}

int LinkStateTable::get_index(long ip_addr)
{
    for (int i = 0; i < link_state_table_.size(); i++)
    {
        if (!link_state_table_.empty() && link_state_table_[i][0].source_ip_addr == ip_addr)
        {
            return i;
        }
    }
    return -1;
}

void LinkStateTable::delete_empty()
{
    for (auto it = link_state_table_.begin(); it != link_state_table_.end(); it++)
    {
        if (it->empty())
        {
            link_state_table_.erase(it);
            continue;
        }
    }
}