from fastapi import FastAPI
from pydantic import BaseModel

import levdes  

app = FastAPI()

class DistanceRequest(BaseModel):
    s1: str
    s2: str
    discount_from: str = "1"  # Puede ser número como "3" o "coda"
    discount_func: str = "log"  # "log" o "exp"
    mode: str = "lev"  # "lev" o "osa"
    vowels: str = "aeiou"

@app.get("/health")
def health_check():
    return {"status": "ok"}

@app.post("/distance")
def compute_distance(request: DistanceRequest):
    distance = levdes.distance(
        request.s1,
        request.s2,
        request.discount_from,
        request.discount_func,
        request.mode,
        request.vowels
    )
    return {"distance": distance}