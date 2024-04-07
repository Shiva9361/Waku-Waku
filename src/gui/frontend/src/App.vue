<template>
  <div class="root">
    <h1>Waku-Waku Simulator</h1>
    <div>
      <button
        type="button"
        style="width: 100px; height: 25px; margin-bottom: 10px"
        @click="show = 0"
      >
        Home
      </button>
      <button
        type="button"
        style="width: 100px; height: 25px; margin-bottom: 10px"
        @click="show = 1"
      >
        Memory
      </button>
      <button
        type="button"
        style="width: 100px; height: 25px; margin-bottom: 10px"
        @click="show = 2"
      >
        Cache
      </button>
    </div>
    <div v-show="show == 0" id="main">
      <div class="content">
        <div class="reghistory">
          <div class="reghistory0">
            <RegisterHistory
              :register_history="register_history_0"
              :counter="counter_0"
              :register_number="0"
            />
          </div>
          <div class="reghistory1">
            <RegisterHistory
              :register_history="register_history_1"
              :counter="counter_1"
              :register_number="1"
            />
          </div>
          <div class="la_text">
            <div class="text">
              <textarea
                class="text"
                spellcheck="false"
                autocapitalize="none"
                autocorrect="off"
                tabindex="0"
                id="code0"
                rows="10"
                cols="10"
              >
              </textarea>
              <textarea
                class="text"
                spellcheck="false"
                autocapitalize="none"
                autocorrect="off"
                id="code1"
                rows="10"
                cols="10"
              >
              </textarea>
            </div>
            <div class="content history">
              <div class="pipelinehistory0">
                <PipelineHistory
                  :pipeline_history="pipeline_history_0"
                  :counter="counter_0"
                  :core_number="0"
                  :is_runnning="is_running"
                />
              </div>
              <div class="pipelinehistory1">
                <PipelineHistory
                  :pipeline_history="pipeline_history_1"
                  :counter="counter_1"
                  :core_number="1"
                  :is_runnning="is_running"
                />
              </div>
            </div>
          </div>
          <div>
            <form method="post">
              <div class="container">
                <h3 style="padding: 10px">Waku-Waku Terminal</h3>
                <div>
                  <label>Addi</label>
                  <input
                    style="margin: 5px"
                    type="number"
                    id="addi"
                    value="1"
                    min="1"
                  />
                </div>
                <div>
                  <label>Add </label>
                  <input
                    style="margin: 5px"
                    type="number"
                    id="add"
                    value="1"
                    min="1"
                  />
                </div>
                <div>
                  <label>Sub </label>
                  <input
                    style="margin: 5px"
                    type="number"
                    id="sub"
                    value="1"
                    min="1"
                  />
                </div>
                <div>
                  <label>Mul </label>
                  <input
                    style="margin: 5px"
                    type="number"
                    id="mul"
                    value="1"
                    min="1"
                  />
                </div>
                <div>
                  <label>Div </label>
                  <input
                    style="margin: 5px"
                    type="number"
                    id="div"
                    value="1"
                    min="1"
                  />
                </div>
                <div>
                  <label>W/O Pipeline</label>
                  <input
                    style="margin: 5px"
                    type="radio"
                    id="no_pipe"
                    name="option"
                  />
                </div>
                <div>
                  <label>Pipelining</label>
                  <input
                    style="margin: 5px"
                    type="radio"
                    id="pipe"
                    name="option"
                    checked
                  />
                </div>
                <div>
                  <label>Forwarding</label>
                  <input
                    style="margin: 5px"
                    type="radio"
                    id="forward"
                    name="option"
                  />
                </div>
                <div>
                  <label>Delay</label>
                  <input
                    style="margin: 5px"
                    type="number"
                    id="delay"
                    value="1000"
                    min="10"
                  />
                </div>
                <div style="margin-top: 5px">
                  Run:
                  <i
                    @click="postRunRequest"
                    class="fa fa-play"
                    aria-hidden="true"
                  ></i>
                </div>
                <div style="margin-top: 5px">
                  Play/Pause-1:
                  <i
                    @click="play_pause_1"
                    id="play_pause_1"
                    class="fa fa-play"
                    aria-hidden="true"
                  ></i>
                </div>
                <div style="margin-top: 5px">
                  Play/Pause-0:
                  <i
                    @click="play_pause_0"
                    id="play_pause_0"
                    class="fa fa-play"
                    aria-hidden="true"
                  ></i>
                </div>
              </div>
            </form>
            <div class="content">
              <div class="container1" v-if="display_stats_0 === true">
                <Stats :stats="stats_0" :core_number="0" />
              </div>
              <div class="container1" v-if="display_stats_1 === true">
                <Stats :stats="stats_1" :core_number="1" />
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
    <div div v-show="show == 1" id="mem">
      <div class="memory"></div>
      <Memory
        :memory="memory"
      />
    </div>
    <div div v-show="show == 2" id="c">
      <div class="cache"></div>
      <CacheHistory
        :cache_history="cache_history"
      />
    </div>
  </div>
</template>

<script>
import PipelineHistory from "./components/PipelineHistory.vue";
import RegisterHistory from "./components/RegisterHistory.vue";
import Stats from "./components/Stats.vue";
import Memory from "./components/Memory.vue"
import CacheHistory from "./components/CacheHistory.vue";
import axios from "axios";
export default {
  name: "App",
  data() {
    return {
      interval: setInterval(() => {}, 1000),
      pipeline_history_0: [],
      register_history_0: [],
      pipeline_history_1: [],
      register_history_1: [],
      stats_0: [],
      stats_1: [],
      counter_0: 0,
      counter_1: 0,
      play_0: false,
      play_1: false,
      no_play_0: true,
      no_play_1: true,
      display_stats_0: false,
      display_stats_1: false,
      delay: 1000,
      is_running: false,
      memory_history: [],
      memory: {},
      mem_counter: 0,
      show: 0,
    };
  },
  components: {
    PipelineHistory,
    RegisterHistory,
    Stats,
    Memory,
    CacheHistory
  },
  methods: {
    async fetchPipelineHistory0() {
      const res = await fetch("http://127.0.0.1:5000/core/0/pipe", {
        credentials: "include",
      });
      const data = await res.json();
      return data;
    },
    async fetchRegisterHistory0() {
      const res = await fetch("http://127.0.0.1:5000/core/0/reg", {});
      const data = await res.json();
      return data;
    },
    async fetchPipelineHistory1() {
      const res = await fetch("http://127.0.0.1:5000/core/1/pipe", {
        credentials: "same-origin",
      });
      const data = await res.json();
      return data;
    },
    async fetchRegisterHistory1() {
      const res = await fetch("http://127.0.0.1:5000/core/1/reg", {
        credentials: "include",
      });
      const data = await res.json();
      return data;
    },
    async fetchStats0() {
      const res = await fetch("http://127.0.0.1:5000/core/0/stats", {
        credentials: "include",
      });
      const data = await res.json();
      return data;
    },
    async fetchStats1() {
      const res = await fetch("http://127.0.0.1:5000/core/1/stats", {
        credentials: "include",
      });
      const data = await res.json();
      return data;
    },
    async fetchMemory() {
      const res = await fetch("http://127.0.0.1:5000/mem", {
        credentials: "include",
      });
      const data = await res.json();
      return data;
    },
    async fetchInitialMemory(){
      const res = await fetch("http://127.0.0.1:5000/initialmem", {
        credentials: "include",
      });
      const data = await res.json();
      console.log(data);
      return data;
    },
    // async initialize(){
    //   for(let i = 0; i < 1024; i++){
    //     this.$set(this.memory, i, 0);
    //   }
    // },
    async postRunRequest() {
      clearInterval(this.interval);
      let pipe = false,
        forward = false;
      if (document.getElementById("pipe").checked) pipe = true;
      if (document.getElementById("forward").checked) {
        pipe = true;
        forward = true;
      }
      try {
        const formData = axios.toFormData({
          pipeline: pipe,
          forward: forward,
          addi: document.getElementById("addi").value,
          add: document.getElementById("add").value,
          div: document.getElementById("div").value,
          sub: document.getElementById("sub").value,
          mul: document.getElementById("mul").value,
          code0: document.getElementById("code0").value,
          code1: document.getElementById("code1").value,
        });
        await axios.post("http://127.0.0.1:5000/run", formData, {
          headers: {
            "Content-Type": "multipart/form-data",
          },
        });
        this.register_history_0 = await this.fetchRegisterHistory0();
        this.pipeline_history_0 = await this.fetchPipelineHistory0();
        this.register_history_1 = await this.fetchRegisterHistory1();
        this.pipeline_history_1 = await this.fetchPipelineHistory1();
        this.memory_history = await this.fetchMemory();
        this.stats_0 = await this.fetchStats0();
        this.stats_1 = await this.fetchStats1();
        this.memory = await this.fetchInitialMemory();
        this.display_stats_0 = false;
        this.display_stats_1 = false;
        this.play_0 = true;
        this.play_1 = true;
        this.counter_0 = 0;
        this.counter_1 = 0;
        this.no_play_0 = false;
        this.no_play_1 = false;
        this.is_running = true;
        this.interval = setInterval(() => {
          if (this.counter_0 >= this.register_history_0.length - 1) {
            this.play_0 = false;
            this.no_play_0 = true;
            this.display_stats_0 = true;
          }
          if (this.play_0) this.counter_0++;
          if (this.counter_1 >= this.register_history_1.length - 1) {
            this.play_1 = false;
            this.no_play_1 = true;
            this.display_stats_1 = true;
          }
          if (this.play_1) this.counter_1++;
          if (this.no_play_0 && this.no_play_1) {
            this.mem_counter = 1;
            clearInterval(this.interval);
          }
          this.update_memory();
        }, document.getElementById("delay").value);
      } catch (error) {
        console.log(error);
      }
    },
    play_pause_0() {
      if (!this.no_play_0) this.play_0 = !this.play_0;
      const ele = document.querySelector("play_pause_0");
      ele.classList.replace("fa-play", "fa-pause");
    },
    play_pause_1() {
      if (!this.no_play_1) this.play_1 = !this.play_1;
    },
    update_memory(){
      if(this.counter_0 in this.memory_history[0]) {
        let res = this.memory_history[0][this.counter_0];
        this.memory[res[0]] = res[1];
      }
      if(this.counter_1 in this.memory_history[1]) {
        let res = this.memory_history[1][this.counter_1];
        this.memory[res[0]] = res[1];
      }
    }
  },
  // async mounted(){
  //   await this.initialize();  
  // },
  async created() {
    fetch("http://127.0.0.1:5000/clear");
    this.register_history_0 = await this.fetchRegisterHistory0();
    this.register_history_1 = await this.fetchRegisterHistory1();
    this.memory_history = await this.fetchMemory();
    this.memory = await this.fetchInitialMemory();
  },
};
</script>

<style>
* {
  box-sizing: border-box;
  margin: 0;
  padding: 0;
  background-color: #141821;
  border-color: steelblue;
  color: antiquewhite;
}

h1 {
  text-align: center;
}
.reghistory {
  display: flex;
  gap: 10px;
}
.reghistory0 .reghistory1 {
  margin: 100px;
}
.pipelinehistory0 .pipelinehistory1 {
  width: 100px;
}
.text {
  margin-top: 1%;
  margin-left: 30px;
  width: 1100px;
  height: 600px;
  display: flex;
  font-size: 20px;
  padding: 5px 0px 0px 5px;
}
.content {
  display: flex;
}
.history {
  padding-left: 55px;
  padding-top: 20px;
  gap: 25px;
}
.container {
  max-width: 350px;
  margin: 20px auto;
  overflow: auto;
  gap: 10px;
  border: 1px solid steelblue;
  padding: 50px;
  border-radius: 5px;
}
.container1 {
  max-width: 350px;
  margin: 20px auto;
  overflow: auto;
  gap: 10px;
  border: 1px solid steelblue;
  padding: 30px;
  border-radius: 5px;
}
</style>
