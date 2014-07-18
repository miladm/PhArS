/*******************************************************************************
 * port.cpp
 *******************************************************************************/

#include "port.h"

template <typename queType_T>
port<queType_T>::port (LENGTH len = 1, CYCLE wire_delay = 1, string port_name) 
	: unit (port_name),
      _buff_len (len),
	  _wire_delay (wire_delay),
      s_port_empty_cyc (g_stats.newScalarStat (port_name, "port_empty_cyc", "Number of cycles with port empty", 0, NO_PRINT_ZERO)),
      s_port_full_cyc  (g_stats.newScalarStat (port_name, "port_full_cyc", "Number of cycles with port full", 0, NO_PRINT_ZERO))
{
	Assert (_buff_len > 0 && _wire_delay > 0);
}

template <typename queType_T>
port<queType_T>::~port () {}

template <typename queType_T>
BUFF_STATE port<queType_T>::pushBack (queType_T ins, CYCLE now) {
	if (getBuffSize () >= _buff_len) {
        dbg.print (DBG_PORT, "%s: %s (cyc: %d)\n", _c_name.c_str (), "is FULL", now);
        return FULL_BUFF;
    }
	BuffElement<queType_T> newBuff (_wire_delay, now, ins);
	_buff.push_back (newBuff);
	return AVAILABLE_BUFF;
}

template <typename queType_T>
BUFF_STATE port<queType_T>::pushBack (queType_T ins, CYCLE now, CYCLE lat) {
	if (getBuffSize () >= _buff_len) {
        dbg.print (DBG_PORT, "%s: %s (cyc: %d)\n", _c_name.c_str (), "is FULL", now);
        return FULL_BUFF;
    }
	BuffElement<queType_T> newBuff (lat, now, ins);
	_buff.push_back (newBuff);
	return AVAILABLE_BUFF;
}

template <typename queType_T>
queType_T port<queType_T>::getFront () {
    Assert(getBuffSize () > 0);
	return (_buff.front ())._dynIns;
}

template <typename queType_T>
queType_T port<queType_T>::getBack () {
    Assert(getBuffSize () > 0);
	return (_buff.back ())._dynIns;
}

template <typename queType_T>
queType_T port<queType_T>::popBack () {
    Assert (getBuffSize () > 0 && "popFront () must not be used without isReady ()");
	queType_T dynIns = (_buff.back ())._dynIns;
	_buff.pop_back ();
    return dynIns;
}

template <typename queType_T>
queType_T port<queType_T>::popFront (CYCLE now) {
    Assert (getBuffSize () > 0 && "popFront () must not be used without isReady ()");
	BuffElement<queType_T> frontBuff = _buff.front ();
	if (frontBuff._delay.getStopTime () <= now)
		return popFront ();
    Assert (true == false  && "popFront () must not be used without isReady ()");
	return popFront (); //place holder
}

template <typename queType_T>
queType_T port<queType_T>::popFront () {
	queType_T dynIns = (_buff.front ())._dynIns;
	_buff.pop_front ();
	return dynIns;
}

template <typename queType_T>
void port<queType_T>::delOldReady (CYCLE now) {
    if (getBuffSize () > 0) return;
    typename list<BuffElement<queType_T> >::iterator it;
    for (it = _buff.begin (); it != _buff.end (); it++) {
        if ((*it)._delay.getStopTime () < now) {
            _buff.erase(it);
        }
    }
}

template <typename queType_T>
queType_T port<queType_T>::popNextReadyNow (CYCLE now) {
    Assert (getBuffSize () > 0 && "popNextReady () must not be used without hasReady ()");
    typename list<BuffElement<queType_T> >::iterator it;
    for (it = _buff.begin (); it != _buff.end (); it++) {
        if ((*it)._delay.getStopTime () == now) {
            queType_T dynIns = (*it)._dynIns;
            _buff.erase(it);
            return dynIns;
        }
    }
    Assert (true == false  && "popNextReady () must not be used without hasReady ()");
	return popFront (); //place holder
}

template <typename queType_T>
queType_T port<queType_T>::popNextReady (CYCLE now) {
    Assert (getBuffSize () > 0 && "popNextReady () must not be used without hasReady ()");
    typename list<BuffElement<queType_T> >::iterator it;
    for (it = _buff.begin (); it != _buff.end (); it++) {
        if ((*it)._delay.getStopTime () <= now) {
            queType_T dynIns = (*it)._dynIns;
            _buff.erase(it);
            return dynIns;
        }
    }
    Assert (true == false  && "popNextReady () must not be used without hasReady ()");
	return popFront (); //place holder
}

template<typename queType_T>
LENGTH port<queType_T>::getBuffSize () {
	return (LENGTH) _buff.size ();
}

template<typename queType_T>
BUFF_STATE port<queType_T>::getBuffState (CYCLE now) {
	if (getBuffSize () >= _buff_len) {
        dbg.print (DBG_PORT, "%s: %s (cyc: %d)\n", _c_name.c_str (), "is FULL", now);
        return FULL_BUFF;
    }
	else if (getBuffSize () == 0) {
        dbg.print (DBG_PORT, "%s: %s (cyc: %d)\n", _c_name.c_str (), "is EMPTY", now);
        return EMPTY_BUFF;
    }
	else {return AVAILABLE_BUFF;}
}

template <typename queType_T>
bool port<queType_T>::isReadyNow (CYCLE now) {
    if (getBuffState (now) == EMPTY_BUFF) {
        dbg.print (DBG_PORT, "%s: %s (cyc: %d)\n", _c_name.c_str (), "is EMPTY", now);
        return false;
    }
	BuffElement<queType_T> frontBuff = _buff.front ();
	if (frontBuff._delay.getStopTime () == now)
        return true;
    return false;
}

template <typename queType_T>
bool port<queType_T>::isReady (CYCLE now) {
    if (getBuffState (now) == EMPTY_BUFF) {
        dbg.print (DBG_PORT, "%s: %s (cyc: %d)\n", _c_name.c_str (), "is EMPTY", now);
        return false;
    }
	BuffElement<queType_T> frontBuff = _buff.front ();
	if (frontBuff._delay.getStopTime () <= now)
        return true;
    return false;
}

template <typename queType_T>
bool port<queType_T>::hasReadyNow (CYCLE now) {
    if (getBuffState (now) == EMPTY_BUFF) {
        dbg.print (DBG_PORT, "%s: %s (cyc: %d)\n", _c_name.c_str (), "is EMPTY", now);
        return false;
    }
    typename list<BuffElement<queType_T> >::iterator it;
    for (it = _buff.begin (); it != _buff.end (); it++) {
        if ((*it)._delay.getStopTime () == now)
            return true;
    }
    return false;
}

template <typename queType_T>
bool port<queType_T>::hasReady (CYCLE now) {
    if (getBuffState (now) == EMPTY_BUFF) {
        dbg.print (DBG_PORT, "%s: %s (cyc: %d)\n", _c_name.c_str (), "is EMPTY", now);
        return false;
    }
    typename list<BuffElement<queType_T> >::iterator it;
    for (it = _buff.begin (); it != _buff.end (); it++) {
        if ((*it)._delay.getStopTime () <= now)
            return true;
    }
    return false;
}

template <typename queType_T>
void port<queType_T>::flushPort (INS_ID elemSeqNum, CYCLE now) {
    if (getBuffSize() == 0) return;
    queType_T elem = getBack();
    while (elem->getInsID () >= elemSeqNum) {
        dbg.print (DBG_PORT, "%s: %s %llu %s %llu (cyc: %d)\n", _c_name.c_str (), 
                                                                "POP ins", elem->getInsID (), 
                                                                "FOR", elemSeqNum, now);
        popBack();
        if (getBuffSize() == 0) break;
        elem = getBack();
    }
}

template <typename queType_T>
void port<queType_T>::regStat (CYCLE now) {
    if (getBuffState (now) == EMPTY_BUFF) {
        s_port_empty_cyc++;
    } else if (getBuffState (now) == FULL_BUFF) {
        s_port_full_cyc++;
    }
}

/** TEMPLATES **/
template struct BuffElement<dynInstruction*>;
template class port<dynInstruction*>;