import { getDatabase, ref, set, onValue, remove } from "firebase/database";
import React, { useState, useEffect } from "react";

export default function Status({db,cn}) {
  const [eatavg, setEatavg] = useState(0);

  useEffect(() => {
    onValue(ref(db, 'eat-avg'), (snapshot) => {
      setEatavg(snapshot.val());
    });
  }, [db]);

  return (
    <div className={cn}>
       Average eating time: {eatavg}
    </div>
  );
}