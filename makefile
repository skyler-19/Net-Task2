objects = main.o Router.o RouteTable.o NextRoutes.o Socket.o

edit : $(objects)
    cc -o edit $(objects) -lwsock32

main.o : Router.h
Router.o : Router.h
RouteTable.o : RouteTable.h
NextRoutes.o : NextRoutes.h
Socket.o : Socket.h

.PHONY : clean
clean :
    rm edit $(objects)