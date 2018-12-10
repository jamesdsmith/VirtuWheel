using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SteeringController : MonoBehaviour
{
    public ThrustmasterWheel wheelObject;
    private float lastRotation = 0;

    public float WheelRange = 180;

    void Start()
    {
    }

    // Update is called once per frame
    void Update()
    {
        if (wheelObject != null)
        {
            float rot = Input.GetAxis("Wheel Angle") * WheelRange;
            wheelObject.wheel.transform.RotateAround(wheelObject.wheel.transform.position,
                wheelObject.wheel.transform.forward,
                rot - lastRotation);
            lastRotation = rot;
        }
    }
}
