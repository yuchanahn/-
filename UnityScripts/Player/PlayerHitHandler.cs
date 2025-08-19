using System;
using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.Events;
using System.Linq;

public class PlayerHitHandler : MonoBehaviour {
    public List<Func<EnemyBehavior, bool>> callback = new List<Func<EnemyBehavior, bool>>();
    public List<Func<GameObject, Vector2, bool>> callback2 = new List<Func<GameObject, Vector2, bool>>();

    bool check_start;
    [SerializeField] LayerMask layer;
    [SerializeField] private bool Parry;
    void FixedUpdate()
    {
        if (check_start)
        {
            var hitColliders = Physics2D.OverlapBoxAll(gameObject.transform.position, box.size / 2, 0, layer);
            if (!hitColliders.Any()) return;
            AttackDetcet(hitColliders.ToList().First());
            check_start = false;
        }
    }

    BoxCollider2D box;
    private void Awake()
    {
        box = GetComponent<BoxCollider2D>();
    }

    public void ColStart()
    {
        check_start = true;
    }

    public void ColEnd()
    {
        check_start = false;
    }

    private void AttackDetcet(Collider2D collision) {
        if (Parry)
        {
            if (collision.GetComponentInParent<EnemyBehavior>())
            {
                callback.TrueForAll(x => x(collision.GetComponentInParent<EnemyBehavior>()));
                //callback2.TrueForAll(f => f(collision.GetComponentInParent<EnemyBehavior>().gameObject,
                //    collision.ClosestPoint(transform.position)));
            }
            return;
        }
        if (collision.GetComponentInParent<EnemyBehavior>()) {
            if(!collision.GetComponentInParent<EnemyBehavior>().isDead)
                callback2.TrueForAll(f => f(collision.GetComponentInParent<EnemyBehavior>().gameObject, collision.ClosestPoint(transform.position)));
            if (!collision.GetComponentInParent<EnemyBehavior>().isDest)
            {
                GameManager.Instance.StartTimeScale(GameManager.Instance.timeScaleItensity);
                GameManager.Instance.CameraShake();
                StartCoroutine(GameManager.Instance.StopTimeScale());
            }
        }
        if (collision.GetComponentInParent<Tube>())
        {
            var collisionPoint = collision.ClosestPoint(transform.position);
            callback2.TrueForAll(f => f(collision.GetComponentInParent<Tube>().gameObject, collisionPoint));
            GameManager.Instance.StartTimeScale(GameManager.Instance.timeScaleItensity);
            GameManager.Instance.CameraShake();
            StartCoroutine(GameManager.Instance.StopTimeScale());
        }
        if (collision.GetComponentInParent<Hammer>())
        {
            var collisionPoint = collision.ClosestPoint(transform.position);
            callback2.TrueForAll(f => f(collision.GetComponentInParent<Hammer>().gameObject, collisionPoint));
            GameManager.Instance.StartTimeScale(GameManager.Instance.timeScaleItensity);
            GameManager.Instance.CameraShake();
            StartCoroutine(GameManager.Instance.StopTimeScale());
        }
        if (collision.GetComponent<IHitable>() is not null) {
            var collisionPoint = collision.ClosestPoint(transform.position);
            callback2.TrueForAll(f => f(collision.transform.gameObject, collisionPoint));
            GameManager.Instance.StartTimeScale(GameManager.Instance.timeScaleItensity);
            GameManager.Instance.CameraShake();
            StartCoroutine(GameManager.Instance.StopTimeScale());
        }
    }
}
