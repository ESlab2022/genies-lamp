import React from "react";
import ControlTableRow from "./ControlTableRow";

export const ControlTable = () => {
  return (
    <div className="overflow-x-auto relative shadow-md w-full">
      <table className="w-full text-sm text-left text-gray-300">
        <thead className="text-xs uppercase bg-gray-700 text-gray-300">
          <tr>
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
            <th scope="col" className="py-3 px-6">
              emergency
            </th>
          </tr>
        </thead>
        <tbody>
          <ControlTableRow
            index={0}
            deviceId="123"
            color="red"
            brightness={100}
            nearby={true}
            emergency={true}
          />
          <ControlTableRow
            index={1}
            deviceId="123"
            color="red"
            brightness={100}
            nearby={true}
            emergency={false}
          />
        </tbody>
      </table>
    </div>
  );
};

export default ControlTable;
