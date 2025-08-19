using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEngine.UI;

public class HitBox : MonoBehaviour
{
    private float damage;

    private void Start()
    {
        damage = GetComponentInParent<EnemyBehavior>().damage;
    }

    public void ON() {
        var b = GetComponent<BoxCollider2D>();
        Physics2D.OverlapBoxAll((Vector2)transform.position + (b.offset * ((GetComponentInParent<Transform>().rotation.y != 0) ? -1 : 1)), b.size, 0)
                 .Where(x => x.gameObject.CompareTag("Player"))
                 .ToList()
                 .ForEach(
            player => {
                var h = GetComponentInParent<Health>();
                h.hps.update_owner(GetComponentInParent<Health>());
                player.GetComponent<Health>().take_damage(damage, transform.position);
            });
    }
}
