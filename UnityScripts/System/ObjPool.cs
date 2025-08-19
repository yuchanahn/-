using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using Unity.VisualScripting;
using UnityEngine;


public class ObjPool : MonoBehaviour {
    static Dictionary<string, GameObject> prefabs = new Dictionary<string, GameObject>();
    static Dictionary<string, List<GameObject>> pool = new Dictionary<string, List<GameObject>>();

    static ObjPool inst = null;


    private void Awake() {
        if (inst is not null) {
            prefabs.Clear();
            pool.Clear();
            //Debug.LogError("objpool error");
            inst = this;
        } else {
            inst = this;
        }
        
        foreach (Transform child in transform) {
            prefabs[child.name] = child.gameObject;
        }
    }

    static public void distory(GameObject obj) {
        obj.SetActive(false);
        obj.transform.SetParent(inst.transform);

        var key = obj.name.Split('(').First();

        if (!pool.ContainsKey(key))
            pool[key] = new List<GameObject>();
        pool[key].Add(obj);
    }
    
    static public GameObject new_obj(string key) {
        var r = Instantiate(prefabs[key], inst.transform);
        r.SetActive(true);
        return r;
    }
    
    void act(IEnumerator c) {
        StartCoroutine(c);
    }

    static public GameObject take(string key, float? timer = null) {
        if (!prefabs.ContainsKey(key)) return null;
        GameObject r;
        if (pool.ContainsKey(key)) {
            r = pool[key].FirstOrDefault();
            if (r) pool[key].RemoveAt(0);
            r?.SetActive(true);
            r ??= new_obj(key);
        } else {
            r = new_obj(key);
        }
        if (timer is not null) inst.act(YCUtils.act_on_over_time(() => distory(r), timer.Value));
        return r;
    }

    
}