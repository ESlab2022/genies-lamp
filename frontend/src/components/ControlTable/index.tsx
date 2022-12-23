import React, { useState } from "react";
import { useQuery } from "@tanstack/react-query";
import ControlTableRow from "./ControlTableRow";
import useToasts from "../../hooks/useToasts";

const devices = [...Array(3).keys()];

export const ControlTable = () => {
  const { pushToast } = useToasts();
  const [prevDeviceState, setPrevDeviceState] = useState<
    Record<string, { emergency: boolean; proximity: boolean }>
  >({
    "0": { emergency: false, proximity: false },
    "1": { emergency: false, proximity: false },
    "2": { emergency: false, proximity: false },
  });
  // fetch data from backend every .5 seconds
  const { data: deviceState, isLoading } = useQuery<
    Record<string, { emergency: boolean; proximity: boolean }>
  >({
    queryKey: ["deviceState"],
    queryFn: () =>
      fetch(`${import.meta.env.VITE_BACKEND_URL}/getState`).then((res) =>
        res.json()
      ),
    refetchInterval: 500,
    onSuccess(data) {
      // compare with previous state
      // if there is a change, push a toast
      Object.keys(data).forEach((deviceID) => {
        if (
          prevDeviceState[deviceID] &&
          prevDeviceState[deviceID].emergency !== data[deviceID].emergency &&
          data[deviceID].emergency
        ) {
          pushToast({
            message: `Device ${deviceID} is in emergency mode`,
            type: "warning",
          });
        }
      });

      setPrevDeviceState(data);
    },
  });

  if (isLoading) {
    return <h1 className="text-white">Loading...</h1>;
  }

  return (
    <>
      <div className="overflow-x-auto relative shadow-md w-full">
        <table className="w-full text-sm text-left text-gray-300">
          <thead className="text-xs uppercase bg-gray-700 text-gray-300">
            <tr>
              <th scope="col" className="py-3 px-3"></th>
              <th scope="col" className="py-3 px-6">
                device id
              </th>
              <th scope="col" className="py-3 px-6">
                Color
              </th>
              <th scope="col" className="py-3 px-6">
                brightness
              </th>
              <th scope="col" className="py-3 px-6">
                nearby
              </th>
            </tr>
          </thead>
          <tbody>
            {devices.map((deviceID, i) => (
              <ControlTableRow
                key={deviceID}
                index={i}
                deviceId={deviceID}
                emergency={
                  deviceState &&
                  deviceState[deviceID] &&
                  deviceState[deviceID].emergency
                }
                nearby={
                  deviceState &&
                  deviceState[deviceID] &&
                  deviceState[deviceID].proximity
                }
              />
            ))}
          </tbody>
        </table>
      </div>
    </>
  );
};

export default ControlTable;
