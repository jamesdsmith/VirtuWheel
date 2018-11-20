using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[Serializable]
public class DataMessage
{
    public int version;
    public List<float> touch_points;
}

public class TouchInteraction : MonoBehaviour
{
    public SerialConnection<DataMessage> serial;
    public string device = "/dev/tty.usbmodem14441";
    public int baudRate = 115200;
    public ThrustmasterWheel wheel;
    public GameObject TouchObjectTemplate;
    private List<GameObject> TouchObjects = new List<GameObject>();

    void Start()
    {
        serial = new SerialConnection<DataMessage>(device, baudRate);
        if (serial != null)
        {
            serial.ReceiveData += OnReceiveData;
        }
        else
        {
            print("Serial connection is not established with interaction manager.");
        }
    }

    // This is for faking data coming from serial, for now
    List<float> fakeMsg = new List<float>() { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    float counter = 0;

    void Update()
    {
        serial.Update();

        counter += Time.deltaTime;
        if (counter > 0.5f)
        {
            fakeMsg.Add(fakeMsg[0]);
            fakeMsg.RemoveAt(0);
            counter = 0;
        }

        OnReceiveData(new DataMessage()
        {
            version = 1,
            touch_points = fakeMsg
        });
    }

    private void OnReceiveData(DataMessage msg)
    {
        List<float> data = msg.touch_points;
        List<int> touchIdxs = new List<int>();
        for (var i = 0; i < data.Count; ++i)
        {
            if (data[i] >= 1)
            {
                touchIdxs.Add(i);
            }
        }

        // next two loops make sure there are the same amoutn of Touch Objects as touch points that we've detected
        for (var i = touchIdxs.Count; i < TouchObjects.Count; ++i)
        {
            TouchObjects.RemoveAt(0);
        }
        for (var i = TouchObjects.Count; i < touchIdxs.Count; ++i)
        {
            TouchObjects.Add(Instantiate(TouchObjectTemplate));
        }

        // Conversion from idx to angle (this may change once we calculate an angle over the average of a set of points)
        float idxToAngle = 10.0f;

        for (var i = 0; i < TouchObjects.Count; ++i)
        {
            TouchObjects[i].transform.position = wheel.wheel.transform.position + wheel.wheel.transform.forward * wheel.wheel.transform.localScale.x / 2;
            TouchObjects[i].transform.RotateAround(wheel.wheel.transform.position, wheel.wheel.transform.up, idxToAngle * touchIdxs[i]);
        }
    }
}
