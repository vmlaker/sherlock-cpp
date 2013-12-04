#ifndef __WORKER_HPP__
#define __WORKER_HPP__

#include <thread>

namespace sherlock {

/*!
  Allows creating thread "objects"
  similarly to Python's Thread (or Process) class.
  For example, in Python you'd create your class this way:

  \code

  from threading import Thread

  class Foo(Thread):
     def __init__(self):
        super(Foo, self).__init__()
     def run(self):
        print('Hello!')

  bar = Foo()
  bar.start()
  bar.join()

  \endcode

  Using Worker class, the analogous C++ definition is:

  \code

  #include <iostream>
  #include "Worker.hpp"

  class Foo : public Worker {
  private:
     void run() { std::cout << "Hello!" << std::endl; }
  };

  int main(int argc, char** argv) {
     bar Foo;
     bar.start();
     bar.join();
  }

  \endcode

 */
class Worker {

public:
    /*!
      Deallocate memory for internal thread object.
     */
    ~Worker()
    {
        if (m_thread) 
        {
            delete m_thread;
        }
    }

    /*!
      Allocate (and start) internal thread object.
    */
    void start ()
    {
        m_thread = new std::thread(&Worker::run, this);
    }

    /*!
      Join internal thread.
    */
    void join ()
    {
        if (m_thread)
        {
            m_thread->join();
        }
    }

private:
    std::thread* m_thread = NULL;

    /*!
      Implement this in subclass.
    */
    virtual void run() = 0;
};

}  // namespace sherlock.

#endif  // __WORKER_HPP__
