using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using UnityEngine.Events;

[Serializable] public class bind_data_t {
    public string name;
    public Action act;
    public UnityEvent ev;
}

public class AnimEvBind : MonoBehaviour
{
    public List<bind_data_t> binds = new List<bind_data_t>();

    public void ani_event(string name) {
        var name_of_event = binds.Find(x => x.name == name);
        if (name_of_event is not null) {
            name_of_event.ev?.Invoke();
            name_of_event.act?.Invoke();
        } else {
            Debug.Log($"No such event NAME : {{{name}}}!!");
        }
    }

    public void AEV_DebugLog(string msg) {
        Debug.Log("AnimEvBind LOG :" +  msg);
    }
}
