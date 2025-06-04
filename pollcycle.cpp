#include "pollcycle.h"
#include "appliancemap.h"
#include <QDateTime>

extern ClientMap clientMap;

void PollCycle() {
	auto& update = clientMap.get<ByNextUpdate>();
	while (true) {
		auto now = QDateTime::currentSecsSinceEpoch();

		//from what has not yet been updated
		auto iter = update.lower_bound(0);

		//to what is scheduled to be updated now
		auto max = update.upper_bound(now);

		while (iter != max) {
			try {
				if (!iter->get()) { // Check if the shared pointer is valid
					iter++;
					continue;
				}

				auto& row = *iter;

				/** the way it works, when an element is updated, the iterator are shifted and is possible is seeing again (even if put the upper bound) */
				if (row->nextUpdate > now) {
					++iter;
					continue;
				}

				// Store the next iterator before modification (else we can either loop or access something wrong as it will be invalidated)
				auto next_iter = iter;
				++next_iter;

				row->lastPoll = now;
				// Modify the element
				update.modify(iter, [&](ClientSP& c) {
					c->nextUpdate = now + 10;
				});

				row->appliance->pollState();

				// Use the stored next iterator
				iter = next_iter;

			} catch (...) {
				iter++;
			}
		}
		sleep(5);
	}
}
