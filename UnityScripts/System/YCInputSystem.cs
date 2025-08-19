using Mono.Cecil.Cil;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.InputSystem;
using UnityEngine.Networking.Types;

public enum EInput_tag {
    none,
    player,
    ui,
}

[Serializable] public class input_ignore_setting_t {
    public EInput_tag tag;
    public bool is_ignore;
}

[Serializable]
public class key_mapping_base_t {
    public string name;
    public EInput_tag tag;
}

[Serializable]
public struct key_pair_t {
    public KeyCode negative;
    public KeyCode positive;
}

[Serializable]
public struct mouse_button_t {
    public int number;
}

[Serializable]
public class axis_mapping1d_t : key_mapping_base_t {
    public key_pair_t[] x;
    public float state;
    public Action<float> ev;
}

[Serializable]
public class axis_mapping2d_t : key_mapping_base_t {
    public key_pair_t[] x;
    public key_pair_t[] y;
    public Vector2 state;
    public Action<Vector2> ev;
}

[Serializable]
public class key_mapping_t : key_mapping_base_t {
    public KeyCode[] key;
    public mouse_button_t[] mouse;
    public bool state;
    public Action<bool> press_ev;
    public Action<bool> unpress_ev;
}

public static class input_sysytem_extension_methods {
    public static T get_event_of_name<T>(this List<T> m, string name) where T : key_mapping_base_t => m.FirstOrDefault(x => x.name == name);
}
public static class EnumUtil<T> {
    public static T Parse(string s)
    {
        return (T)Enum.Parse(typeof(T), s);
    }
}


[Serializable] public struct save_system_data_t
{
    [Serialize] public Dictionary<string, KeyCode[]> key_bind_data;
}

public class YCInputSystem : MonoBehaviour
{
    [SerializeField] public List<axis_mapping1d_t>  axis1d = new List<axis_mapping1d_t>();
    [SerializeField] public List<axis_mapping2d_t>  axis2d = new List<axis_mapping2d_t>();
    [SerializeField] public List<key_mapping_t>     key = new List<key_mapping_t>();
    public List<input_ignore_setting_t> ignore = new List<input_ignore_setting_t>();

    public void add_ignore_setting(string tt)
    {
        var t = EnumUtil<EInput_tag>.Parse(tt);
        var setting = new input_ignore_setting_t
        {
            tag = t,
            is_ignore = true
        };
        if (ignore.Select(x => x.tag == t).Any())
        {
            ignore.First(x => x.tag == t).is_ignore = true;
        }
        else
        {
            ignore.Add(setting);
        }
    }
    
    public void remove_ignore_setting(string tt)
    {
        var t = EnumUtil<EInput_tag>.Parse(tt);
        var setting = new input_ignore_setting_t
        {
            tag = t,
            is_ignore = false
        };
        if (ignore.Select(x => x.tag == t).Any())
        {
            ignore.First(x => x.tag == t).is_ignore = false;
        }
        else
        {
            ignore.Add(setting);
        }
    }

    private void Awake()
    {
        
        var sl = FindObjectOfType<SaveLoad>();

        if (sl is null) return;
        
        var data = sl.Load<save_system_data_t>();
        if (data.HasValue)
        {
            key .Where(x => data.Value.key_bind_data.ContainsKey(x.name))
                .ToList()
                .ForEach(x => x.key = data.Value.key_bind_data[x.name]);
        }
    }

    void Update()
    {
        foreach (var i in axis1d.Where(i => !ignore.Any(t => t.is_ignore && t.tag.Equals(i.tag))))
        {
            i.ev?.Invoke(i.x.Any(x => Input.GetKey(x.positive)) ? 1 : i.x.Any(x => Input.GetKey(x.negative)) ? -1 : 0);
        }
        foreach (var i in axis2d) {
            //Excluding Ignored input system
            if (ignore.Any(t => t.is_ignore && t.tag.Equals(i.tag))) continue;

            Vector2 state = Vector2.zero;
            state.x = i.x.Any(x => Input.GetKey(x.positive)) ? 1 : i.x.Any(x => Input.GetKey(x.negative)) ? -1 : 0;
            state.y = i.y.Any(y => Input.GetKey(y.positive)) ? 1 : i.y.Any(y => Input.GetKey(y.negative)) ? -1 : 0;
            if (i.state != state) i.ev?.Invoke(state);
            i.state = state;
        }
        foreach (var i in key) {
            //Excluding Ignored input system
            if (ignore.Any(t => t.is_ignore && t.tag.Equals(i.tag))) continue;
            bool state = i.state
                ? !i.key.Any(Input.GetKeyUp) || i.mouse.Any(x => Input.GetMouseButtonUp(x.number))
                : i.key.Any(Input.GetKeyDown) || i.mouse.Any(x => Input.GetMouseButtonDown(x.number));
            if (i.state != state) {
                if (state) i.press_ev?.Invoke(true);
                else i.unpress_ev?.Invoke(false);
            }

            i.state = state;
        }
    }
}
