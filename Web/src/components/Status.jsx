import { ref, set, onValue, remove } from "firebase/database";
import React, { useState, useEffect } from "react";

import Log from "./Log"

export default function Status({db,cn,year,month,date,hour,minute,second}) {
  const [eatavg, setEatavg] = useState(0);
  const [logList, setLoglist] = useState([]);
  const [useLed, setUseLed] = useState(false);
  const [useBuzz, setUseBuzz] = useState(false);

  useEffect(() => {
    onValue(ref(db, 'logs'), (snapshot) => {
      const logs = [];
      snapshot.forEach((doc) => {
        logs.push({ id:doc.key, year:doc.val().year, month:doc.val().month, date:doc.val().date, hour:doc.val().hour, minute:doc.val().minute, eatTime:doc.val().eatTime});
      });
      logs.sort((a,b) => {
        return b.id-a.id;
      });
      setLoglist(logs);
    });

    onValue(ref(db, 'eat-avg'), (snapshot) => {
      setEatavg(snapshot.val());
    });

    onValue(ref(db, 'led'), (snapshot) => {
      setUseLed(snapshot.val());
    });

    onValue(ref(db, 'buzzer'), (snapshot) => {
      setUseBuzz(snapshot.val());
    });
  }, [db]);

  async function deleteAlarm(id) {
    remove(ref(db, 'logs/' + id));
  }

  async function updateLed(value) {
    try{
      await set(ref(db, 'led'), value)
    } catch (e) {
      console.error("Error adding document: ", e);
    }
  }

  async function updateBuzz(value) {
    try{
      await set(ref(db, 'buzzer'), value)
    } catch (e) {
      console.error("Error adding document: ", e);
    }
  }

  return (
    <div className={cn}>
      <div className="status__grid">
        <div>
          <h3>Average eating time</h3>
          <h3>{eatavg} seconds</h3>
        </div>
       <div className="settings">
        <div>
          <span>Use LED</span>
          <span>
          <label className="switch"><input type="checkbox" checked={useLed} onChange={() => updateLed(!useLed)}></input><span className="slider"></span></label>
          </span>
        </div>
        <div>
          <span>Use Buzzer</span>
          <span>
          <label className="switch"><input type="checkbox" checked={useBuzz} onChange={() => updateBuzz(!useBuzz)}></input><span className="slider"></span></label>
          </span>
        </div>
       </div>
      </div>
       <div className="log__container">
        <div className="log">
          <span className="log-date">Date<br></br><p>(Y/M/D)</p></span>
          <span className="log-time">Time<br></br><p>(H:M)</p></span>
          <span className="log-eatTime">Eating Time<br></br><p>(seconds)</p></span>
        </div>
        { logList && logList.map( (item, index) =>
          <Log
            index={index}
            year={item.year}
            month={item.month}
            date={item.date}
            minute={item.minute}
            hour={item.hour}
            eatTime={item.eatTime}
            key={item.id}
            id={item.id}
            deleteAlarm={deleteAlarm}
          />
        )}
        <div className="frame"></div>
       </div>
    </div>
  );
}