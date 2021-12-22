//
//  SetTime.swift
//  alarmset
//
//  Created by Kraiwit Charojrochkul on 15/12/2564 BE.
//

import SwiftUI

struct SetTime: View {
    var body: some View {
        HStack(alignment: .center){
            Spacer()
            SelectTime(max: 24, step: 1)
            Spacer()
            SelectTime(max: 60, step: 5)
            Spacer()
        }
        .padding()
        .background(Color("primary"))
    }
}

struct SetTime_Previews: PreviewProvider {
    static var previews: some View {
        SetTime()
    }
}
