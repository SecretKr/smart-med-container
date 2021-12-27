import { initializeApp } from "firebase/app";
import { getDatabase, ref, set, onValue, remove } from "firebase/database";
import firebaseConfig from "../config";
//import { getAuth, signInWithEmailAndPassword } from "firebase/auth";

import React, { useState, useEffect } from "react";
import Clock from "./Clock";
import Alarm from "./Alarm";
import Status from "./Status";
//import Login from "./Login";

function App() {
  const app = initializeApp(firebaseConfig);
  const db = getDatabase(app);
  //const auth = getAuth();

  // const [isLogin, setIsLogin] = useState(1);
  // const [email, setEmail] = useState("");
  // const [pass, setPass] = useState("");
  const [alarmList, setAlarmlist] = useState([]);
  const [page, setPage] = useState(0);

  // const login = () => {
  //   signInWithEmailAndPassword(auth, email, pass)
  //     .then((userCredential) => {
  //       console.log(userCredential.user);
  //     })
  //     .catch((error) => {
  //       console.log(error.code);
  //       console.log(error.message);
  //     })
  //   setIsLogin(1);
  // }

  useEffect(() => {
    onValue(ref(db, 'alarms'), (snapshot) => {
      const alarms = [];
      snapshot.forEach((doc) => {
        alarms.push({ id:doc.key, minute:doc.val().minute, hour:doc.val().hour});
      });
      alarms.sort((a,b) => {
        return a.id-b.id;
      });
      setAlarmlist(alarms);
    });
  }, [db]);

  async function updateList(minute, hour) {
    try{
      await set(ref(db, 'alarms/' + (hour*60+minute)), {
        hour: hour,
        minute: minute
      });
    } catch (e) {
      console.error("Error adding document: ", e);
    }
  }

  async function deleteAlarm(id) {
    remove(ref(db, 'alarms/' + id));
  }

  const changePage = () => {
    if (page == 0) setPage(1);
    else setPage(0);
  }

  return (
    <div className="app">
      <div className="page-selection" onClick={changePage}>
        <h3>Status</h3>
        <h3>Set Alarm</h3>
        <div className={page == 0 ? "page0" : "page1"}></div>
      </div>
      <Status
        cn={page == 0 ? "status active" : "status"}
        db={db}
      />
      <Clock
        cn={page == 1 ? "clock-container active" : "clock-container"}
        updateList={updateList}
      />
      <div className={page == 1 ? "alarm__container active" : "alarm__container"}>
        { alarmList && alarmList.map( (item, index) =>
          <Alarm
            minute={item.minute}
            hour={item.hour}
            key={item.id}
            id={item.id}
            deleteAlarm={deleteAlarm}
          />
        )}
      </div>
    </div>
  );
}

export default App;
