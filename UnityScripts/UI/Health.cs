using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Health : MonoBehaviour
{
    [SerializeField] public float value;
    [SerializeField] public float max_value;
    public Func<float, float> damage_func;
    public Action<float> damage_ev;
    public Action<float, Vector2> damage_ev_with_hitpoint;
    public Action dead_ev;

    [SerializeField] public HpBarList hps;


    private void Awake()
    {

    }

    public void set_hp(float value_)
    {
        value = value_;
        hps.set_slider(this);
    }

    public void SetDie()
    {
        take_damage(value);
    }
    
    public void take_damage(float damage, Vector2? hit_point = null) {
        hps.update_owner(this);
        float d = 0;
        if (damage_func is null) {
            value -= damage;
        } else {
            d = damage_func(damage);
            value -= d;
        }
        damage_ev?.Invoke(damage);
        if (hit_point is not null) {
            var dmg = damage_func is not null ? d : damage;
            damage_ev_with_hitpoint?.Invoke(dmg, hit_point.Value);
        }
        if (value <= 0) {
            dead_ev?.Invoke();
        }
        //dead_ev?.Invoke();
    }
}
