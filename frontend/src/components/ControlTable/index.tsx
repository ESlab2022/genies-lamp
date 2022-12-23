import React from "react";
import { useQuery } from "@tanstack/react-query";
import ControlTableRow from "./ControlTableRow";
import useToasts from "../../hooks/useToasts";

const devices = [...Array(3).keys()];

export const ControlTable = () => {
  const { pushToast } = useToasts();
  // fetch data from backend every .5 seconds
  const { data: emergencyData, isLoading } = useQuery({
    queryKey: ["emergency"],
    queryFn: () =>
      fetch("http://192.168.10.36:4000/getEmergency").then((res) => res.json()),
    refetchInterval: 500,
    onSuccess(data) {
      Object.entries(data).forEach(([deviceID, emergency]) => {
        if (emergency) {
          pushToast({
            type: "warning",
            message: `Device ${deviceID} is in emergency mode`,
          });
        }
      });
    },
  });

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
                emergency={emergencyData && emergencyData[deviceID]}
                nearby={false}
              />
            ))}
          </tbody>
        </table>
      </div>
    </>
  );
};

export default ControlTable;
