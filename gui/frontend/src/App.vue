<template>
  <div class="root">
    <h1>Waku-Waku Simulator</h1>
    <i @click="postRunRequest" class="fa fa-play" aria-hidden="true"></i>
    <i @click="play_pause_0" class="fa fa-play" aria-hidden="true"></i>
    <form method="post">
      <div class="container">
        <h3>Core-0 Arithmetic Instruction Latencies</h3>
        <div>
          <label>Addi</label>
          <input type="number" id="addi" value="1" min="1" />
        </div>
        <div>
          <label>Add </label>
          <input type="number" id="add" value="1" min="1" />
        </div>
        <div>
          <label>Sub </label>
          <input type="number" id="sub" value="1" min="1" />
        </div>
        <div>
          <label>Mul </label>
          <input type="number" id="mul" value="1" min="1" />
        </div>
        <div>
          <label>Div </label>
          <input type="number" id="div" value="1" min="1" />
        </div>
      </div>
    </form>
    <div class="content">
      <div>
        <PipelineHistory
        :pipeline_history="pipeline_history_0"
        :counter="counter_0"
        />
      </div>
      <div>
        <RegisterHistory
        :register_history="register_history_0"
        :counter="counter_0"
        />
      </div>
      <div>
        <PipelineHistory
        :pipeline_history="pipeline_history_1"
        :counter="counter_1"
        />
      </div>
      <div>
        <RegisterHistory
        :register_history="register_history_1"
        :counter="counter_1"
        />
      </div>
      <div class="container" v-if="display_stats_0 === true">
        <Stats 
        :stats="stats_0"
      />
      </div>
      <div>
        <Stats 
        :stats="stats_1"
      />
      </div>
    </div>
    <textarea class="text" id="code0" rows="10" cols="10"> </textarea>
    <textarea class="text" id="code1" rows="10" cols="10"> </textarea>
  </div>
</template>

<script>
import PipelineHistory from "./components/PipelineHistory.vue";
import RegisterHistory from "./components/RegisterHistory.vue";
import Stats from './components/Stats.vue'
import axios from "axios";
export default {
  name: "App",
  data() {
    return {
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
      display_stats_0: false,
      display_stats_1: false,
      delay: 1000,
    };
  },
  components: {
    PipelineHistory,
    RegisterHistory,
    Stats,
  },
  methods: {
    async fetchPipelineHistory0() {
      const res = await fetch("http://localhost:5000/core/0/pipe");
      const data = await res.json();
      return data;
    },
    async fetchRegisterHistory0() {
      const res = await fetch("http://localhost:5000/core/0/reg");
      const data = await res.json();
      return data;
    },
    async fetchPipelineHistory1() {
      const res = await fetch("http://localhost:5000/core/1/pipe");
      const data = await res.json();
      return data;
    },
    async fetchRegisterHistory1() {
      const res = await fetch("http://localhost:5000/core/1/reg");
      const data = await res.json();
      return data;
    },
    async fetchStats0() {
      const res = await fetch("http://localhost:5000/core/0/stats");
      const data = await res.json();
      return data;
    },
    async fetchStats1() {
      const res = await fetch("http://localhost:5000/core/1/stats");
      const data = await res.json();
      return data;
    },
    async postRunRequest() {
      try {
        const formData = axios.toFormData({
          pipeline: "true",
          forward: "true",
          addi: document.getElementById("addi").value,
          add: document.getElementById("add").value,
          div: document.getElementById("div").value,
          sub: document.getElementById("sub").value,
          mul: document.getElementById("mul").value,
          code0: document.getElementById("code0").value,
          code1: document.getElementById("code1").value,
        });
        await axios.post("http://localhost:5000/run", formData, {
          headers: {
            "Content-Type": "multipart/form-data",
          },
        });
        this.register_history_0 = await this.fetchRegisterHistory0();
        this.pipeline_history_0= await this.fetchPipelineHistory0();
        this.register_history_1 = await this.fetchRegisterHistory1();
        this.pipeline_history_1 = await this.fetchPipelineHistory1();
        this.stats_0 = await this.fetchStats0();
        this.stats_1 = await this.fetchStats1();
        this.play_0 = true;
        this.play_1 = true;
      } catch (error) {
        console.log(error);
      }
    },
    play_pause_0() {
      this.play_0 = !this.play_0;
    },
    play_pause_1() {
      this.play_1 = !this.play_1;
    },
  },
  created() {
    setInterval(() => {
      if (this.play_0) this.counter_0++;
      if (this.counter_0 == this.pipeline_history_0.length - 1) {
        this.play_0 = false; // make it non playable after this
        this.display_stats_0 = true;
      }
    }, 1000);
  },
};
</script>

<style scoped>
* {
  box-sizing: border-box;
  margin: 0;
  padding: 0;
}
h1 {
  text-align: center;
}
.text {
  margin-left: 30px;
  width: 500px;
  height: 500px;
  display: inline-block;
}
.content {
  display: inline-block;
}
.container {
  max-width: 250px;
  margin: 30px auto;
  overflow: auto;
  min-height: 150px;
  border: 1px solid steelblue;
  padding: 30px;
  border-radius: 5px;
}
</style>
