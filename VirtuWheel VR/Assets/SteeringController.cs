using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SteeringController : MonoBehaviour
{
    public ThrustmasterWheel wheelObject;
    private float lastRotation = 0;

    void Start()
    {
        if (wheelObject != null)
        {
            wheelObject = wheelObject.GetComponent<ThrustmasterWheel>();
        }
    }

    // Update is called once per frame
    void Update()
    {
        if (wheelObject != null)
        {
            float rot = Input.GetAxis("Wheel Angle") * 450;
            wheelObject.wheel.transform.RotateAround(wheelObject.wheel.transform.position,
                wheelObject.wheel.transform.up,
                rot - lastRotation);
            lastRotation = rot;
        }
    }
}
