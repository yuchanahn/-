using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.Windows;


enum eTypeOfUIControl {
    XAxis,
    YAxis,
}

public class UIView : MonoBehaviour
{
    [SerializeField] RectTransform UIPointer;
    [SerializeField] AnimationCurve lerp_speed;
    int target_idx = 0;

    [SerializeField] List<RectTransform> Slots = new List<RectTransform>();
    YCInputSystem input;
    float t = 0;

    [SerializeField] eTypeOfUIControl type;
    [SerializeField] private UnityEvent onDisableEvent;
    private void Awake() {
        input = FindObjectOfType<YCInputSystem>();

        if (input.ignore.All(i => i.tag != EInput_tag.ui)) {
            input_ignore_setting_t ignore = new input_ignore_setting_t();
            ignore.tag = EInput_tag.ui;
            ignore.is_ignore = true;
            input.ignore.Add(ignore);
        } 
        if(input.ignore.All(i => i.tag != EInput_tag.player)) {
            input_ignore_setting_t ignore2 = new input_ignore_setting_t();
            ignore2.tag = EInput_tag.player;
            ignore2.is_ignore = false;
            input.ignore.Add(ignore2);
        }

        Slots = GetComponentsInChildren<UIControl>()
                .Select(x=>x.GetComponent<RectTransform>())
                .ToList();

        Slots[target_idx].GetComponent<UIControl>().enabled = true;
    }

    input_ignore_setting_t ign(EInput_tag x) => input.ignore.First(i => i.tag == x);

    float uimove_x = 0;
    private void OnEnable() {
        ign(EInput_tag.ui).is_ignore = false;
        ign(EInput_tag.player).is_ignore = true;

        UIPointer.SetParent(transform);
        
        Slots[target_idx].GetComponent<UIControl>().enabled = true;
        Debug.Log($"t_idx = {target_idx}");
        
        if (type == eTypeOfUIControl.YAxis) {
            input.key.get_event_of_name("UI_UP").press_ev = _ => {

                Slots.ForEach(x => x.GetComponent<UIControl>().enabled = false);

                target_idx = Mathf.Max(--target_idx, 0);
                Slots[target_idx].GetComponent<UIControl>().enabled = true;
                t = 0;
            };
            input.key.get_event_of_name("UI_Down").press_ev = _ => {

                Slots.ForEach(x => x.GetComponent<UIControl>().enabled = false);

                target_idx = Mathf.Min(++target_idx, Slots.Count - 1);
                Slots[target_idx].GetComponent<UIControl>().enabled = true;
                t = 0;
            };
        } else if (type == eTypeOfUIControl.XAxis) {

            input.axis1d.get_event_of_name("UI_Move").ev = x => {
                if (x == 0) return;
                if (uimove_x == x) return;
                Slots.ForEach(x => x.GetComponent<UIControl>().enabled = false);

                target_idx = (target_idx + 1) % Slots.Count;
                Slots[target_idx].GetComponent<UIControl>().enabled = true;
                t = 0;
                uimove_x = x;
            };
        }

        Time.timeScale = 0;
    }

    private void OnDisable() {
        ign(EInput_tag.ui).is_ignore = true;
        ign(EInput_tag.player).is_ignore = false;
        Slots.ForEach(x => x.GetComponent<UIControl>().enabled = false);
        Time.timeScale = 1;
        onDisableEvent?.Invoke();
    }

   
    private void Update() {
        t += Time.unscaledDeltaTime;
        UIPointer.position = Vector2.Lerp(UIPointer.position, Slots[target_idx].position, lerp_speed.Evaluate(t));
    }
}
