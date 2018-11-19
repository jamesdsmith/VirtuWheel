using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SteeringController : MonoBehaviour
{
    public GameObject wheelObject;
    private ThrustmasterWheel wheel;
    private float lastRotation = 0;

    void Start()
    {
        if (wheelObject != null)
        {
            wheel = wheelObject.GetComponent<ThrustmasterWheel>();
        }
    }

    // Update is called once per frame
    void Update()
    {
        if (wheel != null)
        {
            float rot = Input.GetAxis("Wheel Angle") * 450;
            wheel.wheel.transform.RotateAround(wheel.wheel.transform.position,
                wheel.wheel.transform.up,
                rot - lastRotation);
            lastRotation = rot;
        }
    }
}
