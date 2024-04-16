import { IdName } from "./id.name";

export interface Parameter {
    id: string;
    label: string;
    value: number | string | boolean;
    type: "slider" | "dropdown" | "switch",
    listValues?: IdName[];
}