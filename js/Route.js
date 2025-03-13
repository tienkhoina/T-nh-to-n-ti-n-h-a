let checkValid = (x, v, Expensive, Timewindow,Weight, Timeserve,Q) => {
    if (v == 0) return true;
    x.push(v);
    let b = 0;
    for (let i = 0; i < x.length - 1; i++) {
        let from = x[i];
        let to = x[i + 1];
        let traveltime = Expensive[from][to];
        if (i == 0) {
            b = max(Timewindow[to][0], traveltime);
        } else {
            b = max(b + Timeserve[from], Timewindow[to][0]);
        }
        b += Timeserve[to];
        if (b > Timewindow[to][1]) {
            return false;
        }
    }
    let sumWeight = 0;
    for (let i = 0; i < x.length; i++) {
        sumWeight += Weight[x[i]];
    }
    if (sumWeight > Q) {
        return false;
    }
    return true;
}

class Route{
    constructor(u, n, Q, Ex, We, Ts, Tw) {
        this.u = u
        this.n = n
        this.Q = Q
        this.Expensive = Ex
        this.Weight = We
        this.Timeserve = Ts
        this.Timewindow = Tw
        this.isValid = true


        this.f1 = this.u.lenght
        this.f2 = 0
        for(let x of this.u){
            for (let i = 0; i < x.length-1; i++) {
                this.f2 += this.Expensive[x[i]][x[i+1]]
            }
        }

        this.f3 = 0

        let b = 0
        for(let x of this.u){
            for (let i = 0; i < x.length-1; i++) {
                let from = x[i]
                let to = x[i + 1]
                let traveltime = this.Expensive[from][to]
                if (i == 0) {
                    b=max(this.Timewindow[to][0], traveltime)
                }
                else {
                    b = max(b + this.Timeserve[from], this.Timewindow[to][0])
                }
                this.f3 = traveltime
                b += this.Timeserve[to]
                
            }       
        }

        for(let x of this.u){
            let sumWeight = 0
            for (let i = 0; i < x.length; i++) {
                sumWeight += this.Weight[x[i]]
            }
            this.f3 += sumWeight
        }
    }



}