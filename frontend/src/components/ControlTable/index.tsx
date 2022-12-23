import React, { useState, useEffect } from "react";
import { useQuery } from "@tanstack/react-query";
import ControlTableRow from "./ControlTableRow";
import { useToast } from "../Toast";

export const ControlTable = () => {
  // fetch data from backend every .5 seconds
  const { data: emergencyData, isLoading } = useQuery({
    queryKey: ["emergency"],
    queryFn: () =>
      fetch("http://192.168.10.36:4000/getEmergency").then((res) => res.json()),
    refetchInterval: 500,
    onSuccess(data) {
      Object.entries(data).forEach(([deviceID, emergency]) => {
        if (emergency) {
          setToastData({
            type: "warning",
            message: `Device ${deviceID} is in emergency mode`,
          });
        }
      });
    },
  });

  const { toast, setToastData } = useToast();

  if (isLoading) return <p>Loading...</p>;

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
            {Object.entries(emergencyData as Record<string, boolean>).map(
              ([key, value], i) => (
                <ControlTableRow
                  key={key}
                  index={i}
                  deviceId={key}
                  emergency={value}
                  nearby={false}
                />
              )
            )}
          </tbody>
        </table>
      </div>
      {toast}
    </>
  );
};

export default ControlTable;
