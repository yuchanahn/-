using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class NormalAttack : MonoBehaviour {
    [SerializeField] protected PlayerHitHandler player_hit_handler;
    [SerializeField] protected float attack_damage;

    protected virtual void Awake() {
        player_hit_handler.callback.Add(init_hit_callback);
        player_hit_handler.callback2.Add(init_hit_callback);
    }

    virtual protected bool init_hit_callback(GameObject enemy, Vector2 pos) {
        return true;
    }

    virtual protected bool init_hit_callback(EnemyBehavior e) {
        bool next = true;
        e.GetComponent<Health>()?.take_damage(attack_damage, transform.position);

        return next;
    }
}
