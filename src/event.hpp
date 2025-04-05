#pragma once
#include <event2/event.h>

// This (ab)uses the "Curiously Recurring Template Pattern" idiom
// see: https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern
// all this cursed nonsense is done to avoid the small cost of virtual functions
// is it premature optimization? yes.

class Event {
    public:
        // when a class `DerivedEvent`` inherits from this class, its operator() method will call this function
        // then, the compiler must deduce the type of the first parameter, which is a pointer to the derived class. 
        // so, the type of `self` is set to `EventDerived`.
        //
        // since `self` is of type EventDerived, the compiler calls EventDerived::operator() instead of Event::operator().
        // this ensures that a derived class with its own definition of this member function gets its member function called appropriately.
        // it has the same effect as using a `virtual` function, just without the overhead of a vtable.
        //
        // this ...sorta elegant? syntax is made possible by C++ 23. previously you'd have to template this class and inherit the template, basically doing Derived : Base<Derived>
    template <typename DerivedEvent>
    void operator()(this DerivedEvent *self, evutil_socket_t fd, short what) {
        self->operator()(fd, what);
    }
};

// How to create your own events:
//
// To create your own event, simply create a class that (1.) inherits from `Event` and (2.) defines its own operator() method
//
//
// For example:
//
//
// class Foo : public Event {
//     public:
//     void operator()(evutil_socket_t fd, short what) {
//         // do some stuff here
//     }
//     // you can have other member functions too!
// };
//
// Whenever Event::operator() is called on an object inheriting from `Event`, the operator() method of the derived class is then called in turn.
// Effectively, Event::operator() is virtual, just without the overhead of virtual functions.
