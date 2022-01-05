import React from "react";

function Log(props) {
  return (
    <div className="log">
      <span className="log-date">{props.year}/{props.month}/{props.date}</span>
      <span className="log-time">{props.hour}:{props.minute}</span>
      <span className="log-eatTime">{props.eatTime}</span>
      <button className="log-delete" onClick={ () => props.deleteAlarm(props.id)}><i className="fas fa-trash"></i></button>
    </div>
  );
}

export default Log;