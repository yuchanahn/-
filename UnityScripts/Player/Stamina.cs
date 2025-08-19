using System.Collections;
using System.Collections.Generic;
using System.Linq;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.UI;
using static UnityEngine.Rendering.DebugUI;

public class Stamina : MonoBehaviour
{
    [SerializeField] public float value;
    [SerializeField] float stemina_max;
    [SerializeField] float stemina_recover_speed;

    [SerializeField] public float stamina_stop_time;


    [SerializeField] bool is_stamina_stop = false;

    [SerializeField] Slider bar;

    
    DJTimer timer;
    Color ui_color;
    [SerializeField] Color exhaustion_color;



    Image fill_img => bar.GetComponentsInChildren<Image>().Where(x => x.name == "Fill").First();

    bool is_stoponce = false;
    private void Awake() {
        timer = new DJTimer(stamina_stop_time);
        timer.Reset();
        ui_color = fill_img.color;
    }

    private void Update() {
        if (!is_stamina_stop && (value <= 10f) && !is_stoponce) {
            is_stamina_stop = true;
            fill_img.color = exhaustion_color;
        }

        if (is_stamina_stop) {
            timer.Tick();
            if (timer.IsEnded) {
                timer.Reset();
                is_stamina_stop = false;
                is_stoponce = true;
                fill_img.color = ui_color;
                value = stemina_max;
            }
        } else {
            if (value < stemina_max) {
                value += stemina_recover_speed * Time.deltaTime;
            }
            if (is_stoponce && (value > 10f)) {
                is_stoponce = false;
            }
        }

        if (bar) {
            bar.value = is_stamina_stop ? (timer.Elapsed / timer.time) : (value / stemina_max);
        }
    }
}
