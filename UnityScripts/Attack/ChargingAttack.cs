using System.Collections;
using System.Collections.Generic;
using Unity.Burst.Intrinsics;
using UnityEngine;

public class ChargingAttack : NormalAttack
{
    [SerializeField] float charge_time;
    [SerializeField] float charge_damage_value;
    [SerializeField] bool is_charging;
    DJTimer timer;

    public bool IsChargeAttackReady => timer.IsEnded;
    public bool IsCharging => is_charging;


    public void start_charge() {
        is_charging = true;
        timer.Reset();
    }

    public void end_charge() {
        //charge_particle.Stop(true, ParticleSystemStopBehavior.StopEmittingAndClear);
        is_charging = false;
    }

    protected override bool init_hit_callback(EnemyBehavior e) {
        e.GetComponent<Health>()?.take_damage(attack_damage * charge_damage_value);
        return true;
    }

    protected override bool init_hit_callback(GameObject e, Vector2 pos) {
        
        e.GetComponent<Health>()?.take_damage(attack_damage * charge_damage_value, transform.position);
        e.GetComponentInParent<Health>()?.take_damage(attack_damage * charge_damage_value, transform.position);
        
        // 피격 이펙트 아직 없기 때문에 안함. 
        //var scl = 1.0f;
        //var eft = ObjPool.take("ChargeAttack_eft", 0.5f);
        //eft.transform.position = pos + (Random.insideUnitCircle * scl);

        return true;
    }
    
    private void Update() {
        timer.Tick();
    }

    protected override void Awake() {
        timer = new DJTimer(charge_time);
        timer.End();
        base.Awake();
    }
}
