using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class UIControl : MonoBehaviour
{
    [SerializeField] Toggle toggle;
    [SerializeField] Slider slider;
    [SerializeField] Button button;

    [Header("°¡¼Óµµ"), SerializeField] AnimationCurve a;

    float t = 0;
    float px = 0;

    private void OnEnable() {
        var input = FindObjectOfType<YCInputSystem>();
        
        Debug.Log($"OnEnabled : {transform.parent.name}|{name}");  
        
        if (toggle) {
            input.key.get_event_of_name("UI_Enter").press_ev = _ => {
                
                toggle.isOn = !toggle.isOn;
            };
        }
        if (slider) {
            input.axis1d.get_event_of_name("UI_Move").ev = x => {
                if (px != x) t = 0;
                t += Time.unscaledDeltaTime;
                slider.value += x * a.Evaluate(Mathf.Clamp01(t)) * Time.unscaledDeltaTime;
                px = x;
            };
        }
        if (button) {
            input.key.get_event_of_name("UI_Enter").press_ev = _ => {
                button.onClick?.Invoke();
                
            };
        }
    }

    private void OnDisable() {
        var input = FindObjectOfType<YCInputSystem>();
        if (input is null) return;
        Debug.Log($"OnDisable : {transform.parent.name}|{name}"); 
        if (toggle) {
            input.key.get_event_of_name("UI_Enter").press_ev = null;
        }
        if (slider) {
            input.axis1d.get_event_of_name("UI_Move").ev = null;
        }
        if (button) {
            input.key.get_event_of_name("UI_Enter").press_ev = null;
        }
    }

    private float tick_t = 0;
    private void Awake() {
        var sound = FindObjectOfType<AudioBinder>();
        if(toggle) toggle.onValueChanged.AddListener(_ => { sound.PlayOneShot("UI_Click", eAudioType.SFX); });
        if(button) button.onClick.AddListener(() => { sound.PlayOneShot("UI_Click", eAudioType.SFX); });
        if(slider) slider.onValueChanged.AddListener(_ =>
        {
            if (tick_t >= 0.1f)
            {
                tick_t = 0;
                sound.PlayOneShot("UI_Volume_Control", eAudioType.SFX);
            }
            else
            {
                tick_t += Time.unscaledDeltaTime;
            }
        });
    }
}
