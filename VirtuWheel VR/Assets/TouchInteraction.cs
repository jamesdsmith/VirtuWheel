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

    public float AngleOffset = 210;
    public float WheelRadius = 0.12f;
    public float TouchThreshold = 6;

    public bool TouchEnabled = false;

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

        //OnReceiveData(new DataMessage()
        //{
        //    version = 1,
        //    touch_points = fakeMsg
        //});
    }

    private float[] thresholds = new float[] {
        5, 5, 5, 5, 5, 5, 5, 6,
        5, 5, 5, 5, 5, 5, 5, 6,
        5, 5, 5, 5, 5, 5, 5, 6,
        5, 5, 5, 5, 5, 5, 5, 6,};
    private void OnReceiveData(DataMessage msg)
    {
        List<float> data = msg.touch_points;
        List<int> touchIdxs = new List<int>();

        // software fix for a buggy sensor
        //if (data[19] <= 1 && data[21] <= 1 && data[20] <= 2)
        //{
        //    data[20] = 1;
        //}
        string datastr = "";
        for (var i = 0; i < data.Count; ++i)
        {
            datastr += data[i] + ", ";
            if (TouchEnabled)
            {
                if (data[i] >= thresholds[i])
                {
                    touchIdxs.Add(i);
                }
            }
            //if (Mathf.Abs(data[i] - thresholds[i])  >= 2)
            //{
            //    touchIdxs.Add(i);
            //}
        }
        print(datastr);

        // next two loops make sure there are the same amoutn of Touch Objects as touch points that we've detected
        int removeAmt = TouchObjects.Count - touchIdxs.Count;
        for (var i = 0; i < removeAmt; ++i)
        {
            Destroy(TouchObjects[0]);
            TouchObjects.RemoveAt(0);
        }
        removeAmt = touchIdxs.Count - TouchObjects.Count;
        for (var i = 0; i < removeAmt; ++i)
        {
            TouchObjects.Add(Instantiate(TouchObjectTemplate, wheel.wheel.transform));
        }

        // Conversion from idx to angle (this may change once we calculate an angle over the average of a set of points)
        float idxToAngle = 11f;

        for (var i = 0; i < TouchObjects.Count; ++i)
        {
            TouchObjects[i].transform.position = wheel.wheel.transform.position + wheel.wheel.transform.up * WheelRadius;
            TouchObjects[i].transform.rotation = Quaternion.identity;
            TouchObjects[i].transform.RotateAround(wheel.wheel.transform.position, 
                wheel.wheel.transform.forward, 
                360 - (idxToAngle * touchIdxs[i] + AngleOffset));
            TouchObjects[i].transform.Rotate(0, 0, -wheel.wheel.transform.eulerAngles.z);
            // TouchObjects[i].transform.localRotation = wheel.wheel.transform.rotation;
        }
    }
}
