#include "audiere.h"
#include "threads.h"

#include <list>
namespace audiere {
  namespace events {

      static Synchronized managerLock;
      static bool threadMustStop = false;
      static bool threadRunning = false;

      std::list<Event*> eventsToPublish;

      class Subscription {
          public:
              Subscriber* subscriber;
              std::string eventType;
      };

      static std::vector<Subscription> subscriptions;

      static void publishOneEvent(const Event& theEvent) {
          std::string type = theEvent.getType(); // store the event type (performance reasons only)

          // go through all subscriptions, if we find a match, trigger the callback
          std::vector<Subscription>::iterator it;
          for(it=subscriptions.begin(); it!=subscriptions.end(); it++) {
              // is this a subscription for the correct type
              if((*it).eventType == type ) {
                  // yes, trigger the callback
                  (*it).subscriber->handleEvent(theEvent);
              }
          }

      }

      // main thread function
      static void publishMessages(void* dummy) {
          while(!threadMustStop) {
              managerLock.lock();
              std::list<Event*>::iterator it;
              for( it=eventsToPublish.begin(); it!=eventsToPublish.end(); it++ ) {
                  publishOneEvent(*(*it));
                  delete *it;
              }
              eventsToPublish.clear();

              managerLock.unlock(); // done, free the lock
              AI_Sleep(50);
          }
          threadRunning = false;
      }

      // destructor, make sure all subscriptions are cancelled
      Subscriber::~Subscriber() {
          Manager::unsubscribeAll(this);
      }

      // subscribe to the given event type
      void Subscriber::subscribeTo(const std::string& eventType) {
          Manager::subscribeTo(this, eventType);
      }

      void Manager::init()
      {
          threadMustStop=false;
          threadRunning=true;
          AI_CreateThread(publishMessages, NULL);
      }

      void Manager::stop()
      {
          threadMustStop=true;
          while( threadRunning ) AI_Sleep(50);
      }

      // removes all subscriptions for the given subscriber
      void Manager::unsubscribeAll(Subscriber* theSubscriber) {
          managerLock.lock(); // make sure we're the only one using global stuff
          std::vector<Subscription>::iterator it;

          // this is a bit of a hassle because an iterator is invalidated by
          // an erase() :(

          bool done = false;
          while( done==false ) {
              bool hitFound = false;
              // go through all subscriptions until we have a match
              for(it=subscriptions.begin(); hitFound==false && it!=subscriptions.end(); it++) {
                  if( (*it).subscriber == theSubscriber ) {
                      // invalidates the iterator, so we need to force a restart
                      subscriptions.erase(it);
                      hitFound = true;
                  }
              }
              // stop looping if we reached the end of the list (no matches found)
              if( it==subscriptions.end() ) {
                  done = true;
              }
          }
          managerLock.unlock(); // let someone else continue
      }
     
      void Manager::subscribeTo(Subscriber* theSubscriber, const std::string& theEventType){
          managerLock.lock(); // mt-safe

          // create a new subscription
          Subscription newSubscription;
          newSubscription.subscriber = theSubscriber;
          newSubscription.eventType = theEventType;

          // register the subscription
          subscriptions.push_back(newSubscription);

          managerLock.unlock(); // done, free the lock
      }

      void Manager::publish(Event* theEvent) {
          if( !threadRunning ) return; // don's store if the publish thread is not running
          managerLock.lock(); // mt-safe
          eventsToPublish.push_back(theEvent);
          managerLock.unlock();
      }

          

  }
}

