//
//  SelectTime.swift
//  alarmset
//
//  Created by Kraiwit Charojrochkul on 15/12/2564 BE.
//

import SwiftUI

struct SelectTime: View {
    @State private var value = 0
    var max: Int
    var step: Int
    
    func incrementStep() {
        value += step
        if value >= max { value = 0 }
    }

    func decrementStep() {
        value -= step
        if value < 0 { value = max-step }
    }

    var body: some View {
        VStack {
            Button(action: incrementStep) {
                Image(systemName: "arrowtriangle.up.fill")
                    .resizable()
                    .frame(width: 32, height: 24)
            }
            .padding(.all, 5.0)
            Text("\(value, specifier: "%02d")")
                .padding(.all, 4.0)
                .font(.custom("Seven Segment", size:96))
                .background(Color("primary2"))
                .cornerRadius(10)
            Button(action: decrementStep) {
                Image(systemName: "arrowtriangle.down.fill")
                    .resizable()
                    .frame(width: 32, height: 24)
            }
            .padding(.all, 5.0)
        }
        .font(/*@START_MENU_TOKEN@*/.title/*@END_MENU_TOKEN@*/)
        .foregroundColor(Color("accent"))
    }
}

struct SelectTime_Previews: PreviewProvider {
    static var previews: some View {
        SelectTime(max: 60, step: 5)
    }
}
