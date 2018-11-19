using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TouchInteraction : MonoBehaviour
{
    public SerialConnection serial;

    void Start()
    {
        if (serial != null)
        {
            serial.ReceiveData += Serial_ReceiveData;
        }
        else
        {
            print("Serial connection is not established with interaction manager.");
        }
    }

    private void Serial_ReceiveData(List<float> data)
    {
        throw new System.NotImplementedException();
    }

    void Update()
    {

    }
}
