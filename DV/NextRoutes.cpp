#include "NextRoutes.h"

void NextRouters::update(Message message);

void NextRouters::push(NextRouter next_router);

NextRouter &NextRouters::operator[](int index)
{
    if (index >= 0 && index < next_routers.size())
        return next_routers[index];
    cerr << "NextRouters: out of range" << endl;
    return *(NextRouter *)NULL;
}
