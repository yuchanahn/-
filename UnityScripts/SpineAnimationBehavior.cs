using System.Collections;
using UnityEngine;
using Spine.Unity;

public class SpineAnimationBehavior : StateMachineBehaviour
{
    public AnimationClip motion;
    string animClip;
    bool loop;

    [Header("스파인 모션 레이어")]
    public int layer = 0;
    public float timeScale = 1.0f;

    private SkeletonAnimation skeletonAnimation;
    private Spine.AnimationState spineAnimationState;
    private Spine.TrackEntry trackEntry;

    private void Awake()
    {
        if (motion != null)
            animClip = motion.name;
        //Debug.Log(animClip);
    }

    public override void OnStateEnter(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
    {
        if(skeletonAnimation == null)
        {
            skeletonAnimation = animator.GetComponentInChildren<SkeletonAnimation>();
            spineAnimationState = skeletonAnimation.state;
        }

        if(animClip != null)
        {
            loop = stateInfo.loop;
            trackEntry = spineAnimationState.SetAnimation(layer, animClip, loop);
            trackEntry.TimeScale = timeScale;
        }
    }
}
