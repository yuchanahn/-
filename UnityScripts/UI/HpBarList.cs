using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class HpBarList : MonoBehaviour
{
    [SerializeField] public Health cur_owner;
    [SerializeField] public Slider bar;
    [SerializeField] public Slider[] AllBar = new Slider[3];
    [SerializeField] bool is_has_cool = false;
    [SerializeField] TextMeshProUGUI name_lable;

    [SerializeField] float cool = 3f;
    [SerializeField] float t = 0;
    public void update_owner(Health owner) {

        if (cur_owner != owner)
        {
            foreach (var bar in AllBar) {
                bar.value = owner.value / owner.max_value;
            }
        }
        
        cur_owner = owner;
        t = 0;
        gameObject.SetActive(true);
        if (name_lable is not null) {
            name_lable.SetText(owner.gameObject.name);
        }
    }

    public void set_slider(Health val)
    {
        foreach (var bar in AllBar) {
            bar.value = val.value / val.max_value;
        }
    }
    
    
    private void Update() {
        if (is_has_cool) {
            t += Time.deltaTime;
        }
        if (t >= cool) {
            gameObject.SetActive(false);
        }
        if (cur_owner is not null) {
            bar.value = cur_owner.value / cur_owner.max_value;
        }
    }
}
