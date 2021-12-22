//
//  ContentView.swift
//  Shared
//
//  Created by Kraiwit Charojrochkul on 15/12/2564 BE.
//

import SwiftUI

struct ContentView: View {
    @State private var hour = 0
    @State private var minute = 0
    
    var body: some View {
        VStack{
            VStack{
                HStack {
                    Spacer()
                    Text("Set your Medicine Alarm")
                        .font(.title)
                        .fontWeight(.semibold)
                        .padding(/*@START_MENU_TOKEN@*/.bottom/*@END_MENU_TOKEN@*/)
                    Spacer()
                }
                .background(/*@START_MENU_TOKEN@*//*@PLACEHOLDER=View@*/Color.blue/*@END_MENU_TOKEN@*/)
                .foregroundColor(/*@START_MENU_TOKEN@*/.white/*@END_MENU_TOKEN@*/)
                SetTime()
            }
            
            Spacer()
        }
        .background(
                LinearGradient(gradient: Gradient(colors: [Color("accent3"), Color("primary")]), startPoint: .topLeading, endPoint: .bottomTrailing)
            )
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
