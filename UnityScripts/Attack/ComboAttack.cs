using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Linq;

public class ComboAttack : NormalAttack
{
    [SerializeField] List<float> combo_damage_value = new List<float>();
    [SerializeField] List<GameObject> combo_efts = new List<GameObject>();
    int cur_combo;

    public void combo_attack(int combo) {
        cur_combo = combo;
        combo_efts[(combo-1) % 3].SetActive(true);
    }

    protected override bool init_hit_callback(GameObject e, Vector2 pos) {
        var combo_idx = (cur_combo - 1) % combo_damage_value.Count;
        e.GetComponent<Health>()?.take_damage(attack_damage * combo_damage_value[combo_idx], transform.position);
        e.GetComponentInParent<Health>()?.take_damage(attack_damage * combo_damage_value[combo_idx], transform.position);
        
        var scl = 1.0f;
        var eft = ObjPool.take("mob_hit_eft", 0.5f).GetComponent<SpriteEffectController>();
        eft.OnAniEnd = () =>
        {
            eft.gameObject.SetActive(false);
        };
        eft.transform.position = pos + (Random.insideUnitCircle * scl);
        return true;
    }
}
