using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ParringAttack : NormalAttack {

    protected override void Awake() {
        player_hit_handler.callback.Add(enemy => {
            if (!enemy.canParry || !enemy.isParry) return true;
            enemy.GetComponent<Health>()?.take_damage(attack_damage);
            enemy.OnParriedHit();
            enemy.canParry = false;
            enemy.isParry = false;
            GetComponentInParent<PC>().OnParrySuccess();
            return false;
        });
    }
}
